#ifndef __C_RAW_VECTOR_DEF_H__
#define __C_RAW_VECTOR_DEF_H__

//prototyping

#include <stddef.h>
#include <stdbool.h>

#define INLINED static inline
#define C_VECTOR_MIN_DEFAULT_CAPACITY 16
//typedef struct _c_vector Vector;

typedef struct _c_vector {
    void   *v_data_ptr;
    size_t  v_elem_size;
    size_t  v_size;     // v_length
    size_t  v_capacity;
} _c_Vector_core;


#endif /*__C_RAW_VECTOR_DEF_H__*/