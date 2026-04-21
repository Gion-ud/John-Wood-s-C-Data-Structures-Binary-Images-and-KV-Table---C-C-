#ifndef KV_TABLE_INTERNAL_H
#define KV_TABLE_INTERNAL_H

#include "kvdef.h"
#include "hash_index.h"
#include <align.h>
#include "local_string.h"
#include <stdbool.h>
#include "allocator.h"

#define KVTBL_MEM_ALIGN 4
#define INLINED static inline
typedef unsigned int uint_t;


INLINED size_t _KVTable_get_total_mem_alloc_size(
    ulong_t     capacity,
    word_t      key_size,
    word_t      val_size,
    word_t      mem_align
) {
    return
        ALIGN_OFFSET(sizeof(byte_t), mem_align) +               // stat_arr
        ALIGN_OFFSET(sizeof(dword_t), mem_align) +              // key_hash_arr
        ALIGN_OFFSET(sizeof(kidx_t) * capacity, mem_align) +    // hidxtbl
        ALIGN_OFFSET(key_size * capacity, mem_align) +          // key_arr    
        ALIGN_OFFSET(val_size * capacity, mem_align);           // val_arr
}

INLINED void _KVTable_deinit(KVTable *kvt_p, struct arena_alloc *alloc_p, uint_t mem_mark) {
    if (!kvt_p || !alloc_p) return;
    alloc_p->setcur(alloc_p->alloc_handle, mem_mark);
    *kvt_p = (KVTable) {0};
}

INLINED uint32_t dword_next_pow2(uint32_t x) {
    if (!x) return 1;
    return 1u << (32 - __builtin_clz(x - 1));
}

INLINED int _KVTable_init(
    KVTable    *kvt_p,
    dword_t     capacity,
    word_t      key_size,
    word_t      val_size,
    struct arena_alloc *alloc_p
) {
    if (!kvt_p || !alloc_p) return KVSTAT_NULL_PTR;
    if (!_IS_POWER_OF_2(capacity))
        capacity = dword_next_pow2(capacity);
    *kvt_p = (KVTable) {0};

    uint_t mem_cur_mark = alloc_p->getcur(alloc_p->alloc_handle);
    kvt_p->stat_arr_p =
        (__typeof__(kvt_p->stat_arr_p))alloc_p->alloc_arr(
            alloc_p->alloc_handle, capacity, sizeof(*kvt_p->stat_arr_p)
        );
    kvt_p->key_hash_arr_p =
        (__typeof__(kvt_p->key_hash_arr_p))alloc_p->alloc_arr(
            alloc_p->alloc_handle, capacity, sizeof(*kvt_p->key_hash_arr_p)
        );
    kvt_p->hidxtbl_p =
        (__typeof__(kvt_p->hidxtbl_p))alloc_p->alloc_arr(
            alloc_p->alloc_handle, capacity, sizeof(*kvt_p->hidxtbl_p)
        );
    kvt_p->key_arr_p =
        alloc_p->alloc_arr(
            alloc_p->alloc_handle, capacity, key_size
        );
    kvt_p->val_arr_p =
        alloc_p->alloc_arr(
            alloc_p->alloc_handle, capacity, val_size
        );

    if (
        !kvt_p->stat_arr_p || !kvt_p->key_hash_arr_p ||
        !kvt_p->hidxtbl_p || !kvt_p->key_arr_p || !kvt_p->val_arr_p 
    ) {
        alloc_p->setcur(alloc_p->alloc_handle, mem_cur_mark);
        return KVSTAT_ALLOC_FAILURE;
    }


    ht_stat_t ret = hash_index_table_init(kvt_p->hidxtbl_p, capacity);
    if (ret < 0) {
        alloc_p->setcur(alloc_p->alloc_handle, mem_cur_mark);
        return KVSTAT_HIT_INIT_FAILURE;
    }

    local_memset(kvt_p->stat_arr_p, 0, sizeof(*kvt_p->stat_arr_p) * capacity);
    local_memset(kvt_p->key_hash_arr_p, 0, sizeof(*kvt_p->key_hash_arr_p) * capacity);
    local_memset(kvt_p->key_arr_p, 0, key_size * capacity);
    local_memset(kvt_p->val_arr_p, 0, val_size * capacity);



    kvt_p->entry_cnt    = 0;
    kvt_p->deleted_cnt  = 0;
    kvt_p->capacity     = capacity;
    kvt_p->key_size     = key_size;
    kvt_p->val_size     = val_size;

    return KVSTAT_SUCCESS;
}


INLINED kidx_t _KVTable_lookup_with_key_hash_raw(KVTable *kvt_p, hash32_t key_hash, void *key_p) { // not checked
    hidx_t hidx = key_hash & (kvt_p->capacity - 1);
    if (kvt_p->hidxtbl_p[hidx] == HT_KIDX_SLOT_EMPTY) return KVSTAT_KEY_NOT_FOUND;

    ulong_t probe_cnt = 0;
    kidx_t kidx = HT_KIDX_SLOT_EMPTY;
    while (probe_cnt < kvt_p->capacity) {
        kidx = kvt_p->hidxtbl_p[hidx];
        if (kidx == HT_KIDX_SLOT_EMPTY) break;
        if (
            kvt_p->stat_arr_p[kidx] != KVT_ENTRY_EMPTY &&
            kvt_p->key_hash_arr_p[kidx] == key_hash &&
            local_memcmp(
                key_p,
                ((byte_t (*)[kvt_p->key_size])kvt_p->key_arr_p)[kidx],
                kvt_p->key_size
            ) == 0
        )
            return kidx;
        ++probe_cnt; ++hidx;
        hidx &= (kvt_p->capacity - 1);
    };
    return KVSTAT_KEY_NOT_FOUND;
}
INLINED kidx_t _KVTable_lookup(KVTable *kvt_p, void *key_p) {
    if (!kvt_p || !key_p) return KVSTAT_NULL_PTR;
    hash32_t key_hash = ht_ops.hash32(key_p, kvt_p->key_size);
    return _KVTable_lookup_with_key_hash_raw(kvt_p, key_hash, key_p);
}

INLINED bool _KVTable_full(KVTable *kvt_p) {
    return
        (kvt_p->entry_cnt * 4 >= kvt_p->capacity * 3) ? true : false;
}
INLINED bool _KVTable_hidxtbl_should_rebuild(KVTable *kvt_p) {
    return
        (kvt_p->deleted_cnt * 2 >= kvt_p->entry_cnt) ? true : false;
}


INLINED int _KVTable_rebuild_hidxtbl(KVTable *kvt_p) { // manual
    if (hash_index_table_init(kvt_p->hidxtbl_p, kvt_p->capacity) < 0)
        return KVSTAT_HIT_INIT_FAILURE;
    for (dword_t i = 0; i < kvt_p->entry_cnt; ++i) {
        if (kvt_p->stat_arr_p[i] != KVT_ENTRY_INUSE) continue;
        hidx_t ret = hash_index_table_insert(
            kvt_p->hidxtbl_p,
            kvt_p->capacity,
            kvt_p->key_hash_arr_p[i],
            i
        );
        if (ret < 0) return KVSTAT_HIT_INSERT_FAILED;
    }
    return KVSTAT_SUCCESS;
}

INLINED kidx_t _KVTable_insert(
    KVTable    *kvt_p,
    void       *key_p,
    void       *val_p
) {
    if (!kvt_p || !key_p || !val_p) return KVSTAT_NULL_PTR;
    if (_KVTable_full(kvt_p)) return KVSTAT_KVT_FULL;

    if (_KVTable_hidxtbl_should_rebuild(kvt_p)) {
        int ret = _KVTable_rebuild_hidxtbl(kvt_p);
        if (ret < 0) return KVSTAT_HIDXTBL_REBUILD_FAILED;
    }

    hash32_t key_hash = ht_ops.hash32(key_p, kvt_p->key_size);
    kidx_t kidx = _KVTable_lookup_with_key_hash_raw(kvt_p, key_hash, key_p);
    if (kidx < 0) {
        kidx = kvt_p->entry_cnt;
        local_memcpy(
            ((byte_t (*)[kvt_p->key_size])kvt_p->key_arr_p)[kidx],
            key_p,
            kvt_p->key_size
        );

        kvt_p->key_hash_arr_p[kidx] = key_hash;

        hidx_t hidx = hash_index_table_insert(
            kvt_p->hidxtbl_p,
            kvt_p->capacity,
            key_hash,
            kidx
        );
        if (hidx < 0) return KVSTAT_HIT_INSERT_FAILED;

        ++kvt_p->entry_cnt;
    }

    local_memcpy(
        ((byte_t (*)[kvt_p->val_size])kvt_p->val_arr_p)[kidx],
        val_p,
        kvt_p->val_size
    );

    if (kvt_p->stat_arr_p[kidx] == KVT_ENTRY_DELETED) --kvt_p->deleted_cnt;
    kvt_p->stat_arr_p[kidx] = KVT_ENTRY_INUSE;

    return kidx;
}

INLINED kidx_t _KVTable_remove(
    KVTable    *kvt_p,
    void       *key_p
) {
    if (!kvt_p || !key_p) return KVSTAT_NULL_PTR;
    kidx_t kidx = _KVTable_lookup(kvt_p, key_p);
    if (kidx < 0) return KVSTAT_KEY_NOT_FOUND;
    kvt_p->stat_arr_p[kidx] = KVT_ENTRY_DELETED;
    ++kvt_p->deleted_cnt;
    return kidx;
}



#endif /*KV_TABLE_INTERNAL_H*/