#include "kvtable_priv.h"

void KVTable_destroy(KVTable *this) {
    if (!this) return;
    KVTable_deinit(this);
    free(this);
}

KVTable *KVTable_create(
    size_t  kvcap,
    size_t  _mem_size
) {
    KVTable *obj_p = (KVTable*)malloc(sizeof(KVTable));
    if (!obj_p) return NULL;
    int kv_errno = KVTable_init(obj_p, kvcap, _mem_size);
    if (kv_errno < 0) {
        KVTable_destroy(obj_p);
        return NULL;
    }
    return obj_p;
}


//new
static inline ssize_t _KVTable_copy(KVTable *kvt_dest_p, KVTable *kvt_src_p) { // this skips deleted entries
    if (
        _KVTable_validate(kvt_dest_p) < 0 ||
        _KVTable_validate(kvt_src_p) < 0
    )
        return KVSTAT_INVALID_THIS;

    LPBuffer key, val;
    for (size_t i = 0; i < kvt_src_p->kvcnt; ++i) {
        if (kvt_src_p->kvtbl_p[i].entry_stat == KV_ENTRY_STAT_DELETED) continue;

        key.len     = kvt_src_p->kvtbl_p[i].key_len;
        key.data    = mem_arena_memcur_to_addr(&kvt_src_p->mem_arena, kvt_src_p->kvtbl_p[i].key_off);
        val.len     = kvt_src_p->kvtbl_p[i].val_len;
        val.data    = mem_arena_memcur_to_addr(&kvt_src_p->mem_arena, kvt_src_p->kvtbl_p[i].val_off);

        kidx_t ret = _KVTable_insert(kvt_dest_p, &key, &val);
        if (ret < 0) return KVSTAT_HIT_INSERT_FAILED;
    }
    return kvt_dest_p->kvcnt;
}

/*
ssize_t KVTable_copy(KVTable *kvt_dest_p, KVTable *kvt_src_p) { // this skips deleted entries
    return _KVTable_copy(kvt_dest_p, kvt_src_p);
}
*/


KVTable *KVTable_compact(KVTable *old_kvt_p) {
    if (_KVTable_validate(old_kvt_p) < 0) return NULL;
    KVTable *new_kvt_p = KVTable_create(
        old_kvt_p->kvcap, old_kvt_p->mem_arena.mem_size
    );
    if (!new_kvt_p) return NULL;
    if (_KVTable_copy(new_kvt_p, old_kvt_p) < 0) {
        KVTable_destroy(new_kvt_p);
        return NULL;
    }
    KVTable_destroy(old_kvt_p);
    return new_kvt_p;
}

static inline bool _KVTable_kvcap_should_resize(
    KVTable *this
) {
    return (this->kvcnt + 1 > this->kvcap) ? true : false;
}

static inline bool _KVTable_memory_should_resize(
    KVTable    *this,
    size_t      key_len,
    size_t      val_len
) {
    return 
        (
            this->mem_arena.mem_cur +
            key_len + val_len + 2 * KVTBL_MEM_ALIGN >
            this->mem_arena.mem_size
        ) ? true : false;
}

static inline bool _KVTable_should_resize(
    KVTable    *this,
    size_t      key_len,
    size_t      val_len
) {
    return
        (
            _KVTable_kvcap_should_resize(this) ||
            _KVTable_memory_should_resize(this, key_len, val_len)
        ) ? true : false;
}

size_t KVTable_size(KVTable *this) {
    if (!this) return 0;
    return this->kvcnt;
}
size_t KVTable_capacity(KVTable *this) {
    if (!this) return 0;
    return this->kvcap;
}
const KVPairView *KVTable_get(KVTable *this, KVPairView *out_kvpv_p, size_t idx) { // explicitly no bound checking
    if (!this || !out_kvpv_p) return NULL;
    out_kvpv_p->key_len = this->kvtbl_p[idx].key_len;
    out_kvpv_p->key_ptr = mem_arena_memcur_to_addr(&this->mem_arena, this->kvtbl_p[idx].key_off);
    out_kvpv_p->val_len = this->kvtbl_p[idx].val_len;
    out_kvpv_p->val_ptr = mem_arena_memcur_to_addr(&this->mem_arena, this->kvtbl_p[idx].val_off);
    return out_kvpv_p;
}

kidx_t KVTable_lookup(
    KVTable            *this,
    LPBuffer           *key_p,
    LPBuffer *const     out_val_p   // you cannot modify
) {
    int st = _KVTable_validate(this);
    if (st < 0) return st;
    if (!key_p || !out_val_p) return KVSTAT_NULL_PTR;

    kidx_t kidx = KVEntry_lookup(
        this->kvtbl_p,
        this->h_idx_tbl_p,
        key_p,
        this->kvcap,
        &this->mem_arena
    );
    if (kidx < 0) return KVSTAT_KEY_NOT_FOUND;


    out_val_p->len  = this->kvtbl_p[kidx].val_len;
    out_val_p->data = mem_arena_memcur_to_addr(&this->mem_arena, this->kvtbl_p[kidx].val_off);

    return kidx;
}
kidx_t KVTable_remove(
    KVTable    *this,
    LPBuffer   *key_p
) {
    int st = _KVTable_validate(this);
    if (st < 0) return st;
    if (!key_p) return KVSTAT_NULL_PTR;

    kidx_t kidx = KVEntry_lookup(
        this->kvtbl_p,
        this->h_idx_tbl_p,
        key_p,
        this->kvcap,
        &this->mem_arena
    );
    if (kidx < 0) return KVSTAT_KEY_NOT_FOUND;

    this->kvtbl_p[kidx].entry_stat = KV_ENTRY_STAT_DELETED;

    return kidx;
}

KVTable *KVTable_insert(
    KVTable    *this,
    LPBuffer   *key_p,
    LPBuffer   *val_p
) {
    if (_KVTable_validate(this) < 0) return NULL;
    if (
        !_KVEntry_valid_lpbuf(key_p) || !_KVEntry_valid_lpbuf(val_p)
    )
        return NULL;
    #include <stdio.h>
    puts("after lpbuf validation");
    if (_KVTable_should_resize(this, key_p->len, val_p->len)) {
        size_t new_kvcap =
            (_KVTable_kvcap_should_resize(this)) ?
            this->kvcap * 2 :
            this->kvcap;
        size_t new_memsize =
            (_KVTable_memory_should_resize(this, key_p->len, val_p->len)) ?
            this->mem_arena.mem_size * 2 :
            this->mem_arena.mem_size;
        KVTable *new_kvt_p = KVTable_create(new_kvcap, new_memsize);
        if (!new_kvt_p) return NULL;
        if (_KVTable_copy(new_kvt_p, this) < 0) {
            KVTable_destroy(new_kvt_p);
            return NULL;
        }
        KVTable_destroy(this);
        this = new_kvt_p;
    };

    printf("after realloc in inserte;;;;;\n");
    LPBuffer out_val;
    if (KVTable_lookup(this, key_p, &out_val) >= 0) KVTable_remove(this, key_p);
    if (_KVTable_insert(this, key_p, val_p) < 0) return NULL;
    return this;
}

