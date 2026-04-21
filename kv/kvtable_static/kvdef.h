#ifndef KV_DEF_H
#define KV_DEF_H

#include <global_intdef.h>

#include "hash_def.h"

enum kv_entry_stat {
    KVT_ENTRY_EMPTY     = 0,
    KVT_ENTRY_INUSE     = 1,
    KVT_ENTRY_DELETED   = 2,
};



typedef struct _kvtblobj {
    byte_t         *stat_arr_p;     // [0]  ; entry state
    dword_t        *key_hash_arr_p; // [1]  ; cached key hash arr
    kidx_t         *hidxtbl_p;      // [2]  ; hash index table
    void           *key_arr_p;      // [3]  ; key arr
    void           *val_arr_p;      // [4]  ; val arr
    dword_t         entry_cnt;      // [5]  ; entry count
    dword_t         deleted_cnt;    // [6]  ; deleted count
    dword_t         capacity;       // [7]  ; entry capacity
    word_t          key_size;       // [8]  ; key size
    word_t          val_size;       // [9]  ; val size
} KVTable;

enum KVStat {
    KVSTAT_SUCCESS                  = 0,
    KVSTAT_NULL_PTR                 = -1,
    KVSTAT_SIZE_NOT_POWER_OF_2      = -2,
    KVSTAT_ALLOC_FAILURE            = -3,
    KVSTAT_HIT_INIT_FAILURE         = -4,
    KVSTAT_KVT_FULL                 = -8,
    KVSTAT_HIT_INSERT_FAILED        = -10,
    KVSTAT_KEY_NOT_FOUND            = -11,
    KVSTAT_ENTRY_DELETED            = -12,
    KVSTAT_OUT_OF_MEMORY            = -13,
    KVSTAT_HIDXTBL_REBUILD_FAILED   = -14,
    KVSTAT_CREATE_FAILURE           = -16,
};



#endif