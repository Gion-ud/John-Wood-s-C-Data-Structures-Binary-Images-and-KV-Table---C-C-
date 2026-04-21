#ifndef ARENA_ALLOC_H
#define ARENA_ALLOC_H

#include "mem_arena.h"
#include <align.h>



extern long mem_arena_init(
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

struct arena_alloc {
    void *alloc_handle;
    void *(*alloc)(void *alloc_handle, size_t alloc_size);
    void *(*alloc_arr)(void *alloc_handle, size_t alloc_cnt, size_t elem_size);
    size_t (*getcur)(void *alloc_handle);
    void (*setcur)(void *alloc_handle, size_t cur);
    void (*reset)(void *alloc_handle); 
};

static inline int arena_alloc_init(
    struct arena_alloc *alloc_p,
    MemArena           *arena_p,
    void               *mem,
    size_t              mem_size,
    size_t              mem_align
) {
    if (!alloc_p || !arena_p || !mem ||
        !mem_size || !IS_POWER_OF_2(mem_align)
    ) return -1;
    if (mem_arena_init(arena_p, mem, mem_size, mem_align) < 0) return -1;
    alloc_p->alloc_handle   = arena_p;
    alloc_p->alloc     	    = (__typeof__(alloc_p->alloc))      &mem_arena_alloc;
    alloc_p->alloc_arr 	    = (__typeof__(alloc_p->alloc_arr))  &mem_arena_alloc_arr;
    alloc_p->getcur    	    = (__typeof__(alloc_p->getcur))     &mem_arena_getcur;
    alloc_p->setcur    	    = (__typeof__(alloc_p->setcur))     &mem_arena_setcur;
    alloc_p->reset     	    = (__typeof__(alloc_p->reset))      &mem_arena_reset;
    return 0;
}

static inline void arena_alloc_deinit(
    struct arena_alloc *alloc_p
) {
    if (!alloc_p) return;
    mem_arena_reset((MemArena*)alloc_p->alloc_handle);
    *alloc_p = (struct arena_alloc) {0};
}


#endif /*ARENA_ALLOC_H*/