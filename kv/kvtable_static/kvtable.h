// kvtable.h
/*
important:
    this is an append only KV Table with capacity * 0.75 being the hard upper limit
    its similar to append only kv storage;
    hash index table, key arr, val arr, entry stat arr and cached key hash arr are 
	separated, each entry is packed tightly toghether;
    hash index is only meant for lookup, and all metadata, key and val are stored
	separately;
    this kvtable comes with a build in mem arena;
    deletion is logical and dead entries are NOT reused;
    hidxtbl automatically rebuilds under high load or high deletion count;
    table does NOT suppor resize;
    this table requires an external mem arena allocator to function and its provided by the user;
*/


#ifndef KV_TABLE_H
#define KV_TABLE_H

#include "kvdef.h"
#include "kvtable_priv.h"

extern void KVTable_deinit(
    KVTable            *kvt_p,
    struct arena_alloc *alloc_p,
    uint_t              mem_mark
);
extern int KVTable_init(
    KVTable            *kvt_p,
    dword_t             capacity,
    word_t              key_size,
    word_t              val_size,
    struct arena_alloc *alloc_p
);
extern size_t KVTable_size(KVTable *kvt_p);
extern ulong_t KVTable_capacity(KVTable *kvt_p);
extern kidx_t KVTable_insert(
    KVTable    *kvt_p,
    void       *key_p,
    void       *val_p
);
extern  void *KVTable_lookup(
    KVTable    *kvt_p,
    void       *key_p
);
extern kidx_t KVTable_remove(
    KVTable    *kvt_p,
    void       *key_p
);


#endif