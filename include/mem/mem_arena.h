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



MA_EXTERN long long mem_arena_init(
    MemArena   *_this,
    void       *mem_buf,
    size_t      mem_size,
    size_t      mem_align
);
MA_EXTERN void *mem_arena_alloc(MemArena *_this, size_t size);
MA_EXTERN void *mem_arena_alloc_memcur(
    MemArena   *_this,
    size_t      size,
    size_t     *out_mem_cur_p
);


MA_INLINED void *mem_arena_memcur_to_addr(
    MemArena   *_this,
    size_t      mem_cur
) {
    if (!_this || !_this->mem_buf || mem_cur > _this->mem_cur) return NULL;
    return (unsigned char*)_this->mem_buf + mem_cur;
}

MA_INLINED size_t mem_arena_getcur(MemArena *_this) {
    if (!_this || !_this->mem_buf) return 0;
    return _this->mem_cur;
}

MA_INLINED size_t mem_arena_setcur(MemArena *_this, size_t prev_pos) {
    if (
        !_this ||
        !_this->mem_buf ||
        prev_pos > _this->mem_cur
    ) return 0;

    _this->mem_cur = prev_pos;
    return _this->mem_cur;
}


MA_INLINED void mem_arena_reset(MemArena *_this) {
    if (!_this ||!_this->mem_buf) return;
    _this->mem_cur = 0;
    _this->mem_alloc_cnt = 0;
    return;
}

#endif