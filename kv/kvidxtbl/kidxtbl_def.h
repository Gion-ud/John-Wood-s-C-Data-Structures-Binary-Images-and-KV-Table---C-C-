#ifndef KEY_INDEX_TABLE_DEF_H
#define KEY_INDEX_TABLE_DEF_H

#include <global_intdef.h>

#include "hash_def.h"
#include <mem/mem_arena.h>

enum kv_entry_stat {
    KVT_ENTRY_EMPTY     = 0,
    KVT_ENTRY_INUSE     = 1,
    KVT_ENTRY_DELETED   = 2,
};

struct kv_pair {
    const char *key_cstr_p; // [0]
    uint32_t    val;        // [1]
};
typedef struct _ht {
    struct kv_pair     *kvtbl_p;        // [0]
    kidx_t             *key_hash_arr_p; // [1]  ; hash arr
    kidx_t             *hidxtbl_p;      // [2]  ; hash index table
    dword_t             kvcnt;          // [3]  ; entry count
} HashTable;

enum KIStat {
    KIST_SUCCESS                = 0,
    KIST_NULL_PTR               = -1,
    KIST_SIZE_NOT_POWER_OF_2    = -2,
    KIST_ALLOC_FAILURE          = -3,
    KIST_HIT_INIT_FAILURE       = -4,
    KIST_KVT_FULL               = -8,
    KIST_HIT_INSERT_FAILED      = -10,
    KIST_KEY_NOT_FOUND          = -11,
    KIST_ENTRY_DELETED          = -12,
    KIST_OUT_OF_MEMORY          = -13,
    KIST_HIDXTBL_REBUILD_FAILED = -14,
    KIST_CREATE_FAILURE         = -16,
};



#endif