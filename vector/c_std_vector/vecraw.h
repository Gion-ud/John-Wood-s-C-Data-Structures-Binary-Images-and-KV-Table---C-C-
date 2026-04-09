#ifndef __C_RAW_VECTOR_H__
#define __C_RAW_VECTOR_H__

#include "vecdef.h"
#include "vecraw.inl"


ssize_t _c_vector_init(
    _c_Vector_core *vec_p,
    size_t          elem_size,
    size_t          capacity
);
void _c_vector_deinit(_c_Vector_core *vec_p);

_c_Vector_core *_c_vector_create(size_t elem_size, size_t capacity);
void _c_vector_destroy(_c_Vector_core *vec_p);


ssize_t _c_vector_init_assign(
    _c_Vector_core *vec_p,
    size_t          elem_size,
    size_t          elem_count,
    void           *elem_arr_p
);
_c_Vector_core *_c_vector_create_assign(
    size_t  elem_size,
    size_t  elem_count,
    void   *elem_arr_p
);


ssize_t _c_vector_push(_c_Vector_core *vec_p, void *elem_p);
ssize_t _c_vector_pop(_c_Vector_core *vec_p);
void    _c_vector_clear(_c_Vector_core *vec_p);
ssize_t _c_vector_reserve(_c_Vector_core *vec_p, size_t size);


INLINED void *_c_vector_get(_c_Vector_core *vec_p, size_t idx);
INLINED void *_c_vector_at(_c_Vector_core *vec_p, size_t idx);
INLINED void *_c_vector_data(_c_Vector_core *vec_p);
INLINED void *_c_vector_begin(_c_Vector_core *vec_p);
INLINED void *_c_vector_end(_c_Vector_core *vec_p);
INLINED void *_c_vector_front(_c_Vector_core *vec_p);
INLINED void *_c_vector_back(_c_Vector_core *vec_p);

INLINED ssize_t _c_vector_size(_c_Vector_core *vec_p);
INLINED ssize_t _c_vector_capacity(_c_Vector_core *vec_p);
INLINED ssize_t _c_vector_empty(_c_Vector_core *vec_p);



#endif /*__C_RAW_VECTOR_H__*/