#include "kvtable_priv.h"

void KVTable_deinit(
    KVTable            *kvt_p,
    struct arena_alloc *alloc_p,
    uint_t              mem_mark
) {
    _KVTable_deinit(kvt_p, alloc_p, mem_mark);
}
int KVTable_init(
    KVTable            *kvt_p,
    dword_t             capacity,
    word_t              key_size,
    word_t              val_size,
    struct arena_alloc *alloc_p
) {
    return _KVTable_init(
        kvt_p,
        capacity,
        key_size,
        val_size,
        alloc_p
    );
}


size_t KVTable_size(KVTable *kvt_p) {
    if (!kvt_p) return 0;
    return kvt_p->entry_cnt;
}
ulong_t KVTable_capacity(KVTable *kvt_p) {
    if (!kvt_p) return 0;
    return kvt_p->capacity;
}
kidx_t KVTable_insert(
    KVTable    *kvt_p,
    void       *key_p,
    void       *val_p
) {
    return _KVTable_insert(kvt_p, key_p, val_p);
}
void *KVTable_lookup(
    KVTable    *kvt_p,
    void       *key_p
) {
    kidx_t kidx = _KVTable_lookup(kvt_p, key_p);
    if (kidx < 0 || kvt_p->stat_arr_p[kidx] != KVT_ENTRY_INUSE) return NULL;
    return (byte_t*)kvt_p->val_arr_p + kidx * kvt_p->val_size;
}
kidx_t KVTable_remove(
    KVTable    *kvt_p,
    void       *key_p
) {
    return _KVTable_remove(kvt_p, key_p);
}

