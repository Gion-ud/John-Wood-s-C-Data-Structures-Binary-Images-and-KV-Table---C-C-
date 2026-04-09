#ifndef __ALLOC_DEF_H__
#define __ALLOC_DEF_H__

#include <stddef.h>

typedef size_t uoff_t;

typedef void (*MEMALLOC_MEM_INIT_FUNC)(void *alloc_handle, size_t mem_size);
typedef void *(*MEMALLOC_MEM_ALLOC_FUNC)(void *alloc_handle, size_t size);
typedef void (*MEMALLOC_MEM_FREE_FUNC)(void *alloc_handle, void *ptr);
typedef void (*MEMALLOC_MEM_DEINIT_FUNC)(void *alloc_handle);
typedef void (*MEMALLOC_MEM_RESET_FUNC)(void *alloc_handle);


struct allocator {
    MEMALLOC_MEM_INIT_FUNC      mem_init;   // void (*)(void *alloc_handle, size_t mem_size)
    MEMALLOC_MEM_ALLOC_FUNC     mem_alloc;  // void *(*)(void *alloc_handle, size_t size)
    MEMALLOC_MEM_FREE_FUNC      mem_free;   // void (*)(void *alloc_handle, void *ptr)
    MEMALLOC_MEM_RESET_FUNC     mem_reset;
    MEMALLOC_MEM_DEINIT_FUNC    mem_deinit;
};



#endif /*__ALLOC_DEF_H__*/