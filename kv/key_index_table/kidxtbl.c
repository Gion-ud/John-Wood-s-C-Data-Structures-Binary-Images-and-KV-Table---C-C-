#include "kidxtbl_priv.h"
#include "kidxtbl.h"

void KeyIndexTable_deinit(
    KeyIndexTable  *kit_p,
    uint_t          mem_mark
) {
    _KeyIndexTable_deinit(kit_p, mem_mark);
}
int KeyIndexTable_init(
    KeyIndexTable      *kit_p,
    dword_t             capacity,
    struct arena_alloc *alloc_p
) {
    return _KeyIndexTable_init(
        kit_p,
        capacity,
        alloc_p
    );
}

kidx_t KeyIndexTable_insert(
    KeyIndexTable  *kit_p,
    const char     *key_cstr_p
) {
    return _KeyIndexTable_insert(kit_p, key_cstr_p);
}

kidx_t KeyIndexTable_lookup(
    KeyIndexTable  *kit_p,
    const char     *key_cstr_p
) {
    kidx_t kidx = _KeyIndexTable_lookup(kit_p, key_cstr_p);
    if (kidx < 0 || kit_p->kidxtbl_p[kidx].state != KVT_ENTRY_INUSE)
        return KIST_KEY_NOT_FOUND;
    return kidx;
}

INLINED const char *_KeyIndexTable_get_key_raw(
    KeyIndexTable  *kit_p,
    uint_t          idx
) {
    return (char*)((MemArena*)kit_p->alloc.alloc_handle)->mem_buf + kit_p->kidxtbl_p[idx].key_off;
}

const char *KeyIndexTable_get_key(
    KeyIndexTable  *kit_p,
    uint_t          idx
) {
    if (idx >= KeyIndexTable_size(kit_p)) return NULL;
    const char *key_cstr_p = _KeyIndexTable_get_key_raw(kit_p, idx);
    kidx_t kidx = _KeyIndexTable_lookup(kit_p, key_cstr_p);
    return
        (kidx < 0 || kit_p->kidxtbl_p[kidx].state != KVT_ENTRY_INUSE)
        ? NULL : key_cstr_p;
}

const char *KeyIndexTable_get_key_raw(
    KeyIndexTable  *kit_p,
    uint_t          idx
) {
    return
        (!kit_p || idx >= KeyIndexTable_size(kit_p))
        ? NULL : _KeyIndexTable_get_key_raw(kit_p, idx);
}


kidx_t KeyIndexTable_remove(
    KeyIndexTable  *kit_p,
    const char     *key_cstr_p
) {
    return _KeyIndexTable_remove(kit_p, key_cstr_p);
}
