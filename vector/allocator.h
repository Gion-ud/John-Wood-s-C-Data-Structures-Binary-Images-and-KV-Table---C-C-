#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include "alloc_def.h"
#include <mem/mem_arena.h>

static inline void mem_free_none(void *alloc_handle, void *ptr) {
    (void)alloc_handle;
    (void)ptr;
}
static inline void mem_deinit_none(void *alloc_handle) {
    (void)alloc_handle;
}

static struct allocator mem_arena_allocator = {
    .mem_init   = (MEMALLOC_MEM_INIT_FUNC)      &mem_arena_init,
    .mem_alloc  = (MEMALLOC_MEM_ALLOC_FUNC)     &mem_arena_alloc,
    .mem_free   = (MEMALLOC_MEM_FREE_FUNC)      &mem_free_none,
    .mem_reset  = (MEMALLOC_MEM_DEINIT_FUNC)    &mem_arena_reset,
    .mem_deinit = (MEMALLOC_MEM_DEINIT_FUNC)    &mem_arena_reset,
};

#include <stdlib.h>




#endif /*__ALLOCATOR_H__*/