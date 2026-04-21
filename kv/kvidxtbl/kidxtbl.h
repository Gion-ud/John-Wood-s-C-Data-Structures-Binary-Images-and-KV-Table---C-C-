// key_index_table/kidxtbl.h
#ifndef KEY_INDEX_TABLE_H
#define KEY_INDEX_TABLE_H

#include "kidxtbl_def.h"

extern void KeyIndexTable_deinit(
    KeyIndexTable  *kit_p,
    uint_t          mem_mark
);
extern int KeyIndexTable_init(
    KeyIndexTable      *kit_p,
    dword_t             capacity,
    struct arena_alloc *alloc_p
);
static inline size_t KeyIndexTable_size(KeyIndexTable *kit_p) {
    if (!kit_p) return 0;
    return kit_p->entry_cnt;
}
static inline ulong_t KeyIndexTable_capacity(KeyIndexTable *kit_p) {
    if (!kit_p) return 0;
    return kit_p->capacity;
}
extern kidx_t KeyIndexTable_insert(
    KeyIndexTable  *kit_p,
    const char     *key_cstr_p
);
extern kidx_t KeyIndexTable_lookup(
    KeyIndexTable  *kit_p,
    const char     *key_cstr_p
);
extern kidx_t KeyIndexTable_remove(
    KeyIndexTable  *kit_p,
    const char     *key_cstr_p
);
extern const char *KeyIndexTable_get_key(
    KeyIndexTable  *kit_p,
    uint_t          idx
);
extern const char *KeyIndexTable_get_key_raw(
    KeyIndexTable  *kit_p,
    uint_t          idx
);

#endif /*KEY_INDEX_TABLE_H*/