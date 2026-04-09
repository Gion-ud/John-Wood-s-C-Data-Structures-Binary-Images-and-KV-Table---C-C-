#include "vector.h"
#include "vecintrnl.inl"
#include <stdlib.h>
#include <string.h>

#define new_arr(T, count) ({\
    T *__arr_p = NULL;\
    do {\
        __arr_p = (T*)malloc((count) * sizeof(T));\
    } while(0);\
    __arr_p\
})
#define delete_arr(arr_p) if (arr_p) free(arr_p)

#define new(T) ({\
    T *__p = NULL;\
    do {\
        __p = (T*)malloc(sizeof(T));\
    } while(0);\
    __p;\
})
#define delete(p) { if (p) free(p); }



INLINED size_t _c_vector_resize_intrnl_(_c_Vector_core *vec_p, size_t new_capacity) {
    void *tmp_data_p = realloc(vec_p->v_data_ptr, new_capacity * vec_p->v_elem_size);
    if (!tmp_data_p) return 0;
    vec_p->v_data_ptr = tmp_data_p;
    vec_p->v_capacity = new_capacity;
    return new_capacity;
}



// ctor && dtor
ssize_t _c_vector_init(
    _c_Vector_core *vec_p,
    size_t          elem_size,
    size_t          capacity
) {
    if (!vec_p || !elem_size) return -1;
//  if (!capacity) capacity = C_VECTOR_MIN_DEFAULT_CAPACITY;

    vec_p->v_elem_size  = elem_size;
    vec_p->v_size       = 0;
    vec_p->v_capacity   = capacity;
    vec_p->v_data_ptr   = malloc(capacity * elem_size);
    if (!vec_p->v_data_ptr) return -1;

    return (ssize_t)capacity;
}
void _c_vector_deinit(_c_Vector_core *vec_p) {
    if (!_c_vector_is_valid_intrnl_(vec_p)) return;
    if (vec_p->v_data_ptr) free(vec_p->v_data_ptr);
    *vec_p = (_c_Vector_core) {0};
    return;
}


void _c_vector_destroy(_c_Vector_core *vec_p) {
    if (!vec_p) return;
    _c_vector_deinit(vec_p);
    delete(vec_p);
    return;
}
_c_Vector_core *_c_vector_create(
    size_t  elem_size,
    size_t  capacity
) {
    if (!elem_size) return NULL;
    if (!capacity) capacity = C_VECTOR_MIN_DEFAULT_CAPACITY;
    _c_Vector_core *vec_p = new(_c_Vector_core);
    if (!vec_p) return NULL;
    if (_c_vector_init(vec_p, elem_size, capacity) < 0) {
        _c_vector_destroy(vec_p);
        return NULL;
    }

    return vec_p;
}



ssize_t _c_vector_init_assign(
    _c_Vector_core *vec_p,
    size_t          elem_size,
    size_t          elem_count,
    void           *elem_arr_p
) {
    if (!vec_p || !elem_size) return -1;
//  if (!capacity) capacity = C_VECTOR_MIN_DEFAULT_CAPACITY;

    vec_p->v_elem_size  = elem_size;
    vec_p->v_size       = elem_count;
    vec_p->v_capacity   = elem_count;
    vec_p->v_data_ptr   = malloc(elem_count * elem_size);
    if (!vec_p->v_data_ptr) return -1;

    memcpy(vec_p->v_data_ptr, elem_arr_p, elem_count * elem_size);

    return (ssize_t)elem_count;
}
_c_Vector_core *_c_vector_create_assign(
    size_t  elem_size,
    size_t  elem_count,
    void   *elem_arr_p
) {
    if (!elem_size || !elem_count || !elem_arr_p) return NULL;
    _c_Vector_core *vec_p = new(_c_Vector_core);
    if (!vec_p) return NULL;
    if (_c_vector_init_assign(vec_p, elem_size, elem_count, elem_arr_p) < 0) {
        _c_vector_destroy(vec_p);
        return NULL;
    }

    return vec_p;
}


//methods
ssize_t _c_vector_push(_c_Vector_core *vec_p, void *elem_p) {
    if (!_c_vector_is_valid_intrnl_(vec_p) || !elem_p) return -1;
    if (_c_vector_is_full_intrnl_(vec_p)) {
        size_t new_cap = (vec_p->v_capacity) ? vec_p->v_capacity * 2 : C_VECTOR_MIN_DEFAULT_CAPACITY;
        if (!_c_vector_resize_intrnl_(vec_p, new_cap)) return -1;
    }
    memcpy(
        _c_vector_end_intrnl_(vec_p),
        elem_p,
        vec_p->v_elem_size
    );
    return ++vec_p->v_size;
}
ssize_t _c_vector_pop(_c_Vector_core *vec_p) {
    if (
        !_c_vector_is_valid_intrnl_(vec_p) ||
        _c_vector_empty_intrnl_(vec_p)
    ) return -1;
    return --vec_p->v_size;
}
void _c_vector_clear(_c_Vector_core *vec_p) {
    if (!_c_vector_is_valid_intrnl_(vec_p)) return;
    vec_p->v_size = 0;
}

ssize_t _c_vector_reserve(_c_Vector_core *vec_p, size_t size) {
    if (!_c_vector_is_valid_intrnl_(vec_p)) return -1;
    if (vec_p->v_capacity < size) {
        size_t new_cap = size;
        if (!_c_vector_resize_intrnl_(vec_p, new_cap)) return -1;
    }
    return (ssize_t)vec_p->v_capacity;
}








// ctor && dtor