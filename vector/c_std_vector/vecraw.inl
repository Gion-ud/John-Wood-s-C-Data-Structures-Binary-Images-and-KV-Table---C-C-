#ifndef __C_RAW_VECTOR_INL__
#define __C_RAW_VECTOR_INL__

//prototyping
#include "vecintrnl.inl"

INLINED void *_c_vector_get(_c_Vector_core *vec_p, size_t idx) { // no bound check
    if (!vec_p) return NULL;
    // if (!_c_vector_is_valid_intrnl_(vec_p) || _c_vector_empty_intrnl_(vec_p)) return NULL;
    return _c_vector_get_intrnl_(vec_p, idx);
}
INLINED void *_c_vector_at(_c_Vector_core *vec_p, size_t idx) { // with bound check
    if (
        !_c_vector_is_valid_intrnl_(vec_p) ||
        _c_vector_empty_intrnl_(vec_p)
    )
        return NULL;
    if (idx >= vec_p->v_size) return NULL;

    return _c_vector_get_intrnl_(vec_p, idx);
}
INLINED void *_c_vector_data(_c_Vector_core *vec_p) {
    if (!vec_p) return NULL;
    return _c_vector_begin_intrnl_(vec_p);
}
INLINED void *_c_vector_begin(_c_Vector_core *vec_p) {
    if (!vec_p) return NULL;
    return _c_vector_begin_intrnl_(vec_p);
}
INLINED void *_c_vector_end(_c_Vector_core *vec_p) {
    if (!vec_p) return NULL;
    return _c_vector_end_intrnl_(vec_p);
}
INLINED void *_c_vector_front(_c_Vector_core *vec_p) {
    if (!vec_p) return NULL;
    return _c_vector_begin_intrnl_(vec_p);
}
INLINED void *_c_vector_back(_c_Vector_core *vec_p) {
    if (!vec_p) return NULL;
    return _c_vector_back_intrnl_(vec_p);
}
INLINED ssize_t _c_vector_size(_c_Vector_core *vec_p) {
    if (!vec_p) return -1;
    return vec_p->v_size;
}
INLINED ssize_t _c_vector_capacity(_c_Vector_core *vec_p) {
    if (!vec_p) return -1;
    return vec_p->v_capacity;
}
INLINED ssize_t _c_vector_empty(_c_Vector_core *vec_p) {
    if (!vec_p) return -1;
    return _c_vector_empty_intrnl_(vec_p);
}


#endif /*__C_RAW_VECTOR_INL__*/