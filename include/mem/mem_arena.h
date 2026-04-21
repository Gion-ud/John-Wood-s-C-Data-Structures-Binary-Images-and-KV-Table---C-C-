#ifndef MEM_ARENA_H
#define MEM_ARENA_H

#include <align.h>
#include <stddef.h>

#define MA_INLINED  static inline
#define MA_EXTERN   extern

//#define MEM_SIZE (64 * 1024)
#define MA_DEFAULT_MEM_ALIGN 16


typedef struct mem_arena {
    void       *mem_buf;
    size_t      mem_align;
    size_t      mem_size;
    size_t      mem_cur;
    size_t      mem_alloc_cnt;
} MemArena;



extern ptrdiff_t mem_arena_init(
    MemArena   *_this,
    void       *mem_buf,
    size_t      mem_size,
    size_t      mem_align
);
extern void *mem_arena_alloc(MemArena *_this, size_t size);
extern void *mem_arena_alloc_arr(MemArena *_this, size_t count, size_t elem_size);
extern size_t mem_arena_getcur(MemArena *_this);
extern void mem_arena_setcur(MemArena *_this, size_t prev_pos);
extern void mem_arena_reset(MemArena *_this);
extern ptrdiff_t mem_arena_get_last_alloc_memcur(MemArena *_this, size_t size);
// This would ONLY work if you call it immediately after allocation
extern ptrdiff_t mem_arena_get_next_alloc_memcur(MemArena *_this, size_t size);

static inline void *mem_arena_memcur_to_addr(MemArena *_this, size_t off) {
    return
        (!_this || !_this->mem_buf) ? NULL :
        (unsigned char*)_this->mem_buf + off;
}
static inline ptrdiff_t mem_arena_addr_to_memcur(MemArena *_this, void *addr) {
    return
        (!_this || !_this->mem_buf || !addr) ? (ptrdiff_t)-1 :
        (ptrdiff_t)addr - (ptrdiff_t)_this->mem_buf;
}

#endif