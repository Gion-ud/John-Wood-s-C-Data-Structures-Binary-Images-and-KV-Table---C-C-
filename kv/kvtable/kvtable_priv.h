#ifndef KV_TABLE_INTERNAL_H
#define KV_TABLE_INTERNAL_H

#include "kvdef.h"
#include "hash_index.h"
#include <mem/mem_arena.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define KVTBL_MEM_ALIGN 4

enum kv_entry_stat {
    KV_ENTRY_STAT_EMPTY     = 0,
    KV_ENTRY_STAT_INUSE     = 1,
    KV_ENTRY_STAT_DELETED   = 2,
};


typedef struct _kv_entry {
    size_t      key_len;
    size_t      key_off;
    size_t      val_len;
    size_t      val_off;
//  void       *key_data;
//  void       *val_data;
    dword_t     key_hash;
    int         entry_stat;
} KVEntry;



typedef struct _kvtblobj {
    KVEntry            *kvtbl_p;        // [0]
    kidx_t             *h_idx_tbl_p;    // [1]
    ulong_t             kvcnt;          // [2]
    ulong_t             kvcap;          // [3]
    void               *mem;            // [4]
    MemArena            mem_arena;      // [5]
} KVTable;


typedef struct _kvtblobj *KVTableHandle;

enum KVStat {
    KVSTAT_SUCCESS              = 0,
    KVSTAT_INVALID_THIS         = -1,
    KVSTAT_SIZE_NOT_POWER_OF_2  = -2,
    KVSTAT_ALLOC_FAILURE        = -3,
    KVSTAT_HIT_INIT_FAILURE     = -4,
    KVSTAT_MA_INIT_FAILURE      = -5,
    KVSTAT_NULL_PTR             = -6,
    KVSTAT_INVALID_MEMSIZE      = -7,
    KVSTAT_KVT_FULL             = -8,
    KVSTAT_INVALID_KVCNT        = -9,
    KVSTAT_HIT_INSERT_FAILED    = -10,
    KVSTAT_KEY_NOT_FOUND        = -11,
    KVSTAT_KVENTRY_DELETED      = -12,
    KVSTAT_OUT_OF_MEMORY        = -13,
    KVSTAT_REALLOC_FAILURE      = -14,
    KVSTAT_INVALID_LPBUFFER     = -15,
    KVSTAT_CREATE_FAILURE       = -16,
    KVSTAT_COPY_KVT_FAILED      = -17,
};

static inline void KVTable_deinit(KVTable *_this) {
    if (!_this) return;
    mem_arena_reset(&_this->mem_arena);

    if (_this->kvtbl_p)     free(_this->kvtbl_p);
    if (_this->h_idx_tbl_p) free(_this->h_idx_tbl_p);
    if (_this->mem)         free(_this->mem);

    *_this = (KVTable) {0};
}
static inline int KVTable_init(
    KVTable        *_this,
    size_t          kvcap,
    size_t          _mem_size
) {
    int st = 0;
    if (!_this) {
        st = KVSTAT_INVALID_THIS;
        goto KVTable_init_failed;
    }
    *_this = (KVTable) {0};
    if (!_mem_size) {
        st = KVSTAT_INVALID_MEMSIZE;
        goto KVTable_init_failed;
    }
    if (!_IS_POWER_OF_2(kvcap)) {
        st = KVSTAT_SIZE_NOT_POWER_OF_2;
        goto KVTable_init_failed;
    }

    _this->kvcnt        = 0;
    _this->kvcap        = kvcap;
    _this->kvtbl_p      = (KVEntry*)malloc(kvcap * sizeof(KVEntry));
    _this->h_idx_tbl_p  = (kidx_t*)malloc(kvcap * sizeof(kidx_t));
    _this->mem          = malloc(_mem_size);

    if (!_this->kvtbl_p || !_this->h_idx_tbl_p || !_this->mem) {
        st = KVSTAT_ALLOC_FAILURE;
        goto KVTable_init_failed;
    }

    memset(_this->kvtbl_p, 0, kvcap * sizeof(KVEntry));

    //memset(_this->h_idx_tbl_p, 0, cap * sizeof(kidx_t));
    if (hash_index_table_init(_this->h_idx_tbl_p, kvcap) < 0) {
        st = KVSTAT_HIT_INIT_FAILURE;
        goto KVTable_init_failed;
    }

    if (mem_arena_init(&_this->mem_arena, _this->mem, _mem_size, KVTBL_MEM_ALIGN) < 0) {
        st = KVSTAT_MA_INIT_FAILURE;
        goto KVTable_init_failed;
    }

    return KVSTAT_SUCCESS;
KVTable_init_failed:
    KVTable_deinit(_this);
    return st;
}

static inline int _KVTable_validate(KVTable *_this) {
    if (!_this) return KVSTAT_INVALID_THIS;
    if (
        !_this->kvtbl_p ||
        !_this->h_idx_tbl_p ||
        !_this->mem
    )
        return KVSTAT_NULL_PTR;
    if (_this->kvcnt > _this->kvcap) return KVSTAT_INVALID_KVCNT;
    return KVSTAT_SUCCESS;
}

#include <stdio.h>

static inline kidx_t KVEntry_lookup(
    KVEntry    *kv_arr,
    kidx_t     *h_idx_tbl_p,
    LPBuffer   *key_p,
    ulong_t     kv_arr_len,
    MemArena   *mem_arena_p
) {
    if (!kv_arr || !h_idx_tbl_p || !key_p || !mem_arena_p) return KVSTAT_NULL_PTR;
    hash32_t key_hash = ht_ops.hash32(key_p->data, key_p->len);
    hidx_t hidx = key_hash & (kv_arr_len - 1);
    if (h_idx_tbl_p[hidx] == HT_KIDX_SLOT_EMPTY) return KVSTAT_KEY_NOT_FOUND;

    ulong_t probe_cnt = 0;
    kidx_t kidx = HT_KIDX_SLOT_EMPTY;
    while (probe_cnt < kv_arr_len) {
        kidx = h_idx_tbl_p[hidx];
        if (kidx < 0) break;
        if (
            kv_arr[kidx].entry_stat == KV_ENTRY_STAT_INUSE &&
            kv_arr[kidx].key_hash == key_hash &&
            kv_arr[kidx].key_len == key_p->len &&
            memcmp(
                mem_arena_memcur_to_addr(
                    mem_arena_p, kv_arr[kidx].key_off
                ),
                key_p->data,
                key_p->len
            ) == 0
        )
            return kidx;
        ++probe_cnt; ++hidx;
        hidx &= (kv_arr_len - 1);
    };
    return KVSTAT_KEY_NOT_FOUND;
}

static inline ssize_t _KVTable_reserve_memory(KVTable *this, size_t mem_size) {
    if (!this) return KVSTAT_INVALID_THIS;
    if (this->mem_arena.mem_size >= mem_size) return this->mem_arena.mem_size;
    void *new_mem = malloc(mem_size);
    if (!new_mem) return KVSTAT_ALLOC_FAILURE;
    memcpy(new_mem, this->mem, this->mem_arena.mem_size);
    free(this->mem);
    this->mem = new_mem;
    this->mem_arena.mem_buf = new_mem;
    this->mem_arena.mem_size = mem_size;
    return (ssize_t)mem_size;
}

#include <stdio.h>

static inline kidx_t _KVTable_insert(
    KVTable    *this,
    LPBuffer   *key_p,
    LPBuffer   *val_p
) {
    if (!this || !key_p || !val_p) return KVSTAT_NULL_PTR;
    

    kidx_t kidx = this->kvcnt;
    size_t mem_cur_pre_alloc = mem_arena_getcur(&this->mem_arena);
    void *key_data_p = NULL, *val_data_p = NULL;

    key_data_p = mem_arena_alloc_memcur(&this->mem_arena, key_p->len + 1, &this->kvtbl_p[kidx].key_off);
    val_data_p = mem_arena_alloc_memcur(&this->mem_arena, val_p->len + 1, &this->kvtbl_p[kidx].val_off);
    if (!key_data_p || !val_data_p) {
        mem_arena_setcur(&this->mem_arena, mem_cur_pre_alloc);
        return KVSTAT_ALLOC_FAILURE;
    }


    hash32_t key_hash = ht_ops.hash32(key_p->data, key_p->len);
    ulong_t kvcnt = 0; // placeholder, ignored
    hidx_t hidx = hash_index_table_insert(
        this->h_idx_tbl_p,
        this->kvcap,
        &kvcnt,
        key_hash,
        kidx
    );
    if (hidx < 0) {
        mem_arena_setcur(&this->mem_arena, mem_cur_pre_alloc);
        return KVSTAT_HIT_INSERT_FAILED;
    }


    this->kvtbl_p[kidx].key_hash   = key_hash;
    this->kvtbl_p[kidx].entry_stat = KV_ENTRY_STAT_INUSE;
    this->kvtbl_p[kidx].key_len    = key_p->len;
    this->kvtbl_p[kidx].val_len    = val_p->len;
    memcpy(key_data_p, key_p->data, key_p->len);
    memcpy(val_data_p, val_p->data, val_p->len);
    ((byte_t*)key_data_p)[key_p->len] = 0; // NUL termination for cstr compatibility
    ((byte_t*)val_data_p)[val_p->len] = 0; // NUL termination for cstr compatibility

    ++this->kvcnt;

    return kidx;
}

#include <stdbool.h>
static inline bool _KVEntry_valid_lpbuf(LPBuffer *buf_p) {
    if (!buf_p || !buf_p->len || !buf_p->data) return false;
    return true;
}



#endif /*KV_TABLE_INTERNAL_H*/