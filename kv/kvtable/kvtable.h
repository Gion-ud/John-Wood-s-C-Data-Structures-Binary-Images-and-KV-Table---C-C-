// kvtable.h
/*
important:
    this is not a regular hash table;
    its similar to append only kv storage;
    hash index and kv entry table is separated and kv entry table is
    linear and packed tightly;
    hash index is only meant for lookup, and all metadata, key and val are stored in kv entry;
    this kvtable comes with a build in mem arena;
    deletion is logical and dead entries are NOT reused and later eliminated via gc;
    manual compaction is required;
    table will automatically resize when out of mem and capacity;
    kv will be deleted from memory arena when insertion fails to avoid memory leak;
    this tale uses open addressing and linear probe;
    ** kv capacity MUST BE power of 2 otherwise it rejects;
    the table itself will automatically resize when running out of mem in mem arena or kcap
*/


#ifndef KV_TABLE_H
#define KV_TABLE_H

#include "kvdef.h"

typedef struct _kvtblobj *KVTableHandle;


#define MEM_SIZE (64 * 1024U)


extern void KVTable_destroy(KVTableHandle kvt_h);
extern KVTableHandle KVTable_create(
    size_t  kvcap,
    size_t  _mem_size
);

extern KVTableHandle KVTable_insert(
    KVTableHandle   kvt_h,
    LPBuffer       *key_p,
    LPBuffer       *val_p
);
extern kidx_t KVTable_lookup(
    KVTableHandle       kvt_h,
    LPBuffer           *key_p,
    LPBuffer *const     out_val_p   // you cannot modify
);
extern kidx_t KVTable_remove(
    KVTableHandle   kvt_h,
    LPBuffer       *key_p
);

/*
extern ssize_t KVTable_copy(
    KVTableHandle   kvt_dest_h,
    KVTableHandle   kvt_src_h
);
*/

extern KVTableHandle KVTable_compact(
    KVTableHandle   old_kvt_h
);

extern size_t KVTable_size(KVTableHandle kvt_h);
extern size_t KVTable_capacity(KVTableHandle kvt_h);
extern const KVPairView *KVTable_get(KVTableHandle kvt_h, KVPairView *out_kvpv_p, size_t idx);
#define new_KVTable(kvcap, _mem_size) ({\
    KVTableHandle __h = (KVTableHandle)0; \
    do {\
        __h = KVTable_create(kvcap, _mem_size);\
    } while(0);\
    __h;\
})
#define delete_KVTable(kvh) \
    do {\
        KVTable_destroy(kvh);\
    } while(0);\



#endif