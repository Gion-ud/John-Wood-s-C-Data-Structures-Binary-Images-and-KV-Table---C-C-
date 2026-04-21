#ifndef KEY_INDEX_TABLE_INTERNAL_H
#define KEY_INDEX_TABLE_INTERNAL_H

#include "kidxtbl_def.h"
#include "hash_index.h"
#include <align.h>
#include "local_string.h"
#include <stdbool.h>

#define KVTBL_MEM_ALIGN 4
#define INLINED static inline
typedef unsigned int uint_t;


INLINED void _KeyIndexTable_deinit(
    KeyIndexTable  *kit_p,
    uint_t          mem_mark
) {
    if (!kit_p) return;
    if (kit_p->alloc.setcur)
        kit_p->alloc.setcur(kit_p->alloc.alloc_handle, mem_mark);
    *kit_p = (KeyIndexTable) {0};
}

INLINED uint32_t dword_next_pow2(uint32_t x) {
    if (!x) return 1;
    return 1u << (32 - __builtin_clz(x - 1));
}

INLINED int _KeyIndexTable_init(
    KeyIndexTable      *kit_p,
    dword_t             capacity,
    struct arena_alloc *alloc_p
) {
    if (!kit_p || !alloc_p) return KIST_NULL_PTR;
    if (!_IS_POWER_OF_2(capacity))
        capacity = dword_next_pow2(capacity);
    *kit_p = (KeyIndexTable) {0};

    uint_t mem_cur_mark = alloc_p->getcur(alloc_p->alloc_handle);
    kit_p->kidxtbl_p =
        (__typeof__(kit_p->kidxtbl_p))alloc_p->alloc_arr(
            alloc_p->alloc_handle, capacity, sizeof(*kit_p->kidxtbl_p)
        );

    kit_p->hidxtbl_p =
        (__typeof__(kit_p->hidxtbl_p))alloc_p->alloc_arr(
            alloc_p->alloc_handle, capacity, sizeof(*kit_p->hidxtbl_p)
        );

    if (!kit_p->kidxtbl_p || !kit_p->hidxtbl_p) {
        alloc_p->setcur(alloc_p->alloc_handle, mem_cur_mark);
        return KIST_ALLOC_FAILURE;
    }

    ht_stat_t ret = hash_index_table_init(kit_p->hidxtbl_p, capacity);
    if (ret < 0) {
        alloc_p->setcur(alloc_p->alloc_handle, mem_cur_mark);
        return KIST_HIT_INIT_FAILURE;
    }

    local_memset(kit_p->kidxtbl_p, 0, sizeof(*kit_p->kidxtbl_p) * capacity);

    kit_p->entry_cnt    = 0;
    kit_p->deleted_cnt  = 0;
    kit_p->capacity     = capacity;
    kit_p->alloc        = *alloc_p;

    return KIST_SUCCESS;
}



INLINED kidx_t _KeyIndexTable_lookup_with_key_hash(
    KeyIndexTable  *kit_p,
    hash32_t        key_hash,
    const char     *key_p
) { // not checked
    hidx_t hidx = key_hash & (kit_p->capacity - 1);
    if (kit_p->hidxtbl_p[hidx] == HIDXTBL_NULL_SLOT)
        return KIST_KEY_NOT_FOUND;

    ulong_t probe_cnt = 0;
    kidx_t kidx = KEY_IDX_NULL;
    while (probe_cnt < kit_p->capacity) {
        kidx = kit_p->hidxtbl_p[hidx];
        if (kidx == HIDXTBL_NULL_SLOT) break;
        if (
            kit_p->kidxtbl_p[kidx].state != KVT_ENTRY_EMPTY &&
            kit_p->kidxtbl_p[kidx].key_hash == key_hash &&
            local_strcmp(
                key_p,
                (char*)((MemArena*)kit_p->alloc.alloc_handle)->mem_buf +
                    kit_p->kidxtbl_p[kidx].key_off
            ) == 0
        )
            return kidx;
        ++probe_cnt; ++hidx;
        hidx &= (kit_p->capacity - 1);
    };
    return KIST_KEY_NOT_FOUND;
}
INLINED kidx_t _KeyIndexTable_lookup(
    KeyIndexTable  *kit_p,
    const char     *key_cstr_p
) {
    if (!kit_p || !key_cstr_p) return KIST_NULL_PTR;
    size_t key_len = local_strlen(key_cstr_p);
    hash32_t key_hash = ht_ops.hash32((const byte_t*)key_cstr_p, key_len);
    return _KeyIndexTable_lookup_with_key_hash(
        kit_p, key_hash, key_cstr_p
    );
}

INLINED bool _KeyIndexTable_full(KeyIndexTable *kit_p) {
    return
        (kit_p->entry_cnt * 4 >= kit_p->capacity * 3) ? true : false;
}
INLINED bool _KeyIndexTable_hidxtbl_should_rebuild(KeyIndexTable *kit_p) {
    return
        (kit_p->deleted_cnt * 2 >= kit_p->entry_cnt) ? true : false;
}

INLINED int _KeyIndexTable_rebuild_hidxtbl(KeyIndexTable *kit_p) { // manual
    if (hash_index_table_init(kit_p->hidxtbl_p, kit_p->capacity) < 0)
        return KIST_HIT_INIT_FAILURE;
    for (dword_t i = 0; i < kit_p->entry_cnt; ++i) {
        if (kit_p->kidxtbl_p[i].state != KVT_ENTRY_INUSE) continue;
        hidx_t ret = hash_index_table_insert(
            kit_p->hidxtbl_p,
            kit_p->capacity,
            kit_p->kidxtbl_p[i].key_hash,
            i
        );
        if (ret < 0) return KIST_HIT_INSERT_FAILED;
    }
    return KIST_SUCCESS;
}

INLINED kidx_t _KeyIndexTable_insert(
    KeyIndexTable  *kit_p,
    const char     *key_cstr_p
) {
    if (!kit_p || !key_cstr_p) return KIST_NULL_PTR;
    if (_KeyIndexTable_full(kit_p)) return KIST_KVT_FULL;

    if (_KeyIndexTable_hidxtbl_should_rebuild(kit_p)) {
        int ret = _KeyIndexTable_rebuild_hidxtbl(kit_p);
        if (ret < 0) return KIST_HIDXTBL_REBUILD_FAILED;
    }
    size_t key_len = local_strlen(key_cstr_p);

    hash32_t key_hash = ht_ops.hash32((const byte_t*)key_cstr_p, key_len);
    kidx_t kidx = _KeyIndexTable_lookup_with_key_hash(
        kit_p, key_hash, key_cstr_p
    );
    if (kidx < 0) {
        kidx = kit_p->entry_cnt;
        
        uint32_t mem_cur_mark = kit_p->alloc.getcur(kit_p->alloc.alloc_handle);
        char *cstr_p = kit_p->alloc.alloc(
            kit_p->alloc.alloc_handle, key_len + 1
        ); // cstr compatibility
        if (!cstr_p) return KIST_ALLOC_FAILURE;
        uint32_t key_off = kit_p->alloc.getcur(kit_p->alloc.alloc_handle) - (key_len + 1);
        // key_off = memcur_after_alloc - (cstr_len + 1) since memory allocation can
        // internally align the starting off of the allocation so memcur_before_alloc
        // is NOT guaranteed to be equal (unless mem_align is 1)

        hidx_t hidx = hash_index_table_insert(
            kit_p->hidxtbl_p,
            kit_p->capacity,
            key_hash,
            kidx
        );

        if (hidx < 0) {
            kit_p->alloc.setcur(kit_p->alloc.alloc_handle, mem_cur_mark);
            return KIST_HIT_INSERT_FAILED;
        }

        local_memcpy(cstr_p, key_cstr_p, key_len);
        cstr_p[key_len] = '\0'; // NUL termination

        kit_p->kidxtbl_p[kidx].key_hash = key_hash;
        kit_p->kidxtbl_p[kidx].state    = KVT_ENTRY_INUSE;
        kit_p->kidxtbl_p[kidx].key_off  = key_off;

        ++kit_p->entry_cnt;
    }

    if (kit_p->kidxtbl_p[kidx].state == KVT_ENTRY_DELETED) --kit_p->deleted_cnt;
    kit_p->kidxtbl_p[kidx].state = KVT_ENTRY_INUSE;

    return kidx;
}

INLINED kidx_t _KeyIndexTable_remove(
    KeyIndexTable  *kit_p,
    const char     *key_cstr_p
) {
    if (!kit_p || !key_cstr_p) return KIST_NULL_PTR;
    kidx_t kidx = _KeyIndexTable_lookup(kit_p, key_cstr_p);
    if (kidx < 0) return KIST_KEY_NOT_FOUND;
    kit_p->kidxtbl_p[kidx].state = KVT_ENTRY_DELETED;
    ++kit_p->deleted_cnt;
    return kidx;
}



#endif /*KEY_INDEX_TABLE_INTERNAL_H*/