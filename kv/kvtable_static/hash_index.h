#ifndef HASH_INDEX_H
#define HASH_INDEX_H

// This provide a BARE BONE hash lookup table
// actual key, data and metadata are stored in separate tables
// the hash table only stores idx ref to the data table entry
// the hash table is NOT src of truth
// it is designed to be disposable
// lookup is dependent on external implm

#include "hash_fn.h"
#include "hash_def.h"
#include <global_intdef.h>

struct ht_ops {
    hash64_t  (*hash64)(const byte_t *key, size_t key_len);
    hash32_t  (*hash32)(const byte_t *key, size_t key_len);
};

static const struct ht_ops ht_ops = {
    .hash64 = &fnv_1a_hash64,
    .hash32 = &fnv_1a_hash32,
};

#define HT_KIDX_SLOT_EMPTY -1

typedef enum _ht_stat {
    HIT_STAT_SUCCESS            = 0,
    HIT_STAT_INVALID_HIDXTBL    = -1,
    HIT_STAT_SIZE_NOT_POWEROF2  = -2,
    HIT_STAT_ALLOC_FAILURE      = -3,
    HIT_STAT_HT_FULL            = -4,
    HIT_STAT_NOT_FOUND          = -5,
    HIT_STAT_NULL_PTR           = -7,
} ht_stat_t;


#define _IS_POWER_OF_2(num) (\
    ((num) > 0) && \
    (((num) & ((num) - 1)) == 0)\
)

static inline ht_stat_t hash_index_table_init(
    kidx_t     *h_idx_tbl_p,
    ulong_t     ht_size
) {
    if (!h_idx_tbl_p) return HIT_STAT_INVALID_HIDXTBL;
    if (!_IS_POWER_OF_2(ht_size)) return HIT_STAT_SIZE_NOT_POWEROF2;

    for (size_t i = 0; i < ht_size; ++i) {
        h_idx_tbl_p[i] = HT_KIDX_SLOT_EMPTY;
    }

    return HIT_STAT_SUCCESS;
}

static inline hidx_t hash_index_table_insert(
    kidx_t     *h_idx_tbl_p,
    ulong_t     ht_size,
    hash32_t    key_hash,   // key hash provided by usr
    kidx_t      k_idx       // key idx in the kv arr
) {
    if (!h_idx_tbl_p) return HIT_STAT_INVALID_HIDXTBL;
    if (!_IS_POWER_OF_2(ht_size)) return HIT_STAT_SIZE_NOT_POWEROF2;

    hidx_t h_idx = key_hash & (ht_size - 1);

    ulong_t probe_cnt = 0;
    while (probe_cnt < ht_size) {
        if (h_idx_tbl_p[h_idx] == HT_KIDX_SLOT_EMPTY) {
            h_idx_tbl_p[h_idx] = k_idx;
            return HIT_STAT_SUCCESS;
        }
        ++h_idx; ++probe_cnt;
        h_idx &= (ht_size - 1);
    }

    return h_idx;
}


#endif