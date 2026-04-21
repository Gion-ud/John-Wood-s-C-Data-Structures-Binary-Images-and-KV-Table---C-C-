#ifndef KEY_INDEX_TABLE_DEF_H
#define KEY_INDEX_TABLE_DEF_H

#include <global_intdef.h>

#include "hash_def.h"
#include <mem/arena_alloc.h>

enum kv_entry_stat {
    KVT_ENTRY_EMPTY     = 0,
    KVT_ENTRY_INUSE     = 1,
    KVT_ENTRY_DELETED   = 2,
};

typedef struct key_index_entry {
    uint32_t    key_hash;   // [0]
    uint32_t    key_off;    // [1]
    uint32_t    state;      // [2]
} KeyIndexEntry;
typedef struct _kidxtbl {
    KeyIndexEntry      *kidxtbl_p;      // [0]
    kidx_t             *hidxtbl_p;      // [1]  ; hash index table
    dword_t             entry_cnt;      // [2]  ; entry count
    dword_t             deleted_cnt;    // [3]  ; deleted count
    dword_t             capacity;       // [4]  ; entry capacity
    struct arena_alloc  alloc;          // [5]  ; allocator
} KeyIndexTable;

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