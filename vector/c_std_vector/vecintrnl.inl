#ifndef __C_RAW_VECTOR_INTRNL_INL__
#define __C_RAW_VECTOR_INTRNL_INL__

//prototyping

#include "vecdef.h"


INLINED bool _c_vector_is_valid_intrnl_(_c_Vector_core *vec_p) {
    if (
        !vec_p ||
        !vec_p->v_data_ptr ||
        !vec_p->v_elem_size ||
//      !vec_p->v_capacity ||
        vec_p->v_size > vec_p->v_capacity
    )
        return false;
    return true;
}
INLINED bool _c_vector_is_full_intrnl_(_c_Vector_core *vec_p) { // internal, no checking
    if (
        vec_p->v_size >= vec_p->v_capacity
    )
        return true;
    return false;
}
INLINED bool _c_vector_empty_intrnl_(_c_Vector_core *vec_p) {
    if (!_c_vector_is_valid_intrnl_(vec_p) || !vec_p->v_size) return true;
    return false;
}
INLINED void *_c_vector_get_intrnl_(_c_Vector_core *vec_p, size_t idx) { // no bound check
    return (void*)((uintptr_t)vec_p->v_data_ptr + idx * vec_p->v_elem_size);
}
INLINED void *_c_vector_begin_intrnl_(_c_Vector_core *vec_p) {
    return (void*)((uintptr_t)vec_p->v_data_ptr);
}
INLINED void *_c_vector_end_intrnl_(_c_Vector_core *vec_p) {
    return (void*)((uintptr_t)vec_p->v_data_ptr + vec_p->v_size * vec_p->v_elem_size);
}
INLINED void *_c_vector_back_intrnl_(_c_Vector_core *vec_p) {
    return (void*)((uintptr_t)vec_p->v_data_ptr + (vec_p->v_size - 1) * vec_p->v_elem_size);
}



#endif /*__C_RAW_VECTOR_INTRNL_INL__*/