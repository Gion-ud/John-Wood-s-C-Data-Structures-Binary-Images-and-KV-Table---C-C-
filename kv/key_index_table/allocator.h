#ifndef __KVTABLE_ALLOCATOR_H__
#define __KVTABLE_ALLOCATOR_H__

#include <global_intdef.h>

struct arena_alloc {
    void *alloc_handle;
    void *(*alloc)(void *alloc_handle, size_t alloc_size);
    void *(*alloc_arr)(void *alloc_handle, size_t alloc_cnt, size_t elem_size);
    size_t (*getcur)(void *alloc_handle);
    void (*setcur)(void *alloc_handle, size_t cur);
//  void *(*free)(void *alloc_handle, void *addr);
    void (*reset)(void *alloc_handle); 
};

#endif /*__KVTABLE_ALLOCATOR_H__*/