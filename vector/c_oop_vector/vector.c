#include <stddef.h>
//#include "allocator.h"
//#include "alloc_def.h"
#include <global_intdef.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "vector.h"
#include <sys/types.h>

static inline ssize_t _Vector_resize_raw(Vector *vec_p) {
/*
    if (
        !vec_p || !vec_p->vec_buf_ptr
    )
        return false;
*/
    size_t new_size = vec_p->vec_max_elem_cnt * 2;
    void *tmp = realloc(vec_p->vec_buf_ptr, new_size * vec_p->vec_elem_size);
    if (!tmp) return -1;
    vec_p->vec_max_elem_cnt = new_size;
    vec_p->vec_buf_ptr = tmp;
    return new_size;
}

// Methods 
bool Vector_push_raw(
    Vector *vec_p,
    void   *itm_p
) {
    if (
        !vec_p || !itm_p || !vec_p->vec_buf_ptr
    )
        return false;
    if (vec_p->vec_len >= vec_p->vec_max_elem_cnt) {
        if (_Vector_resize_raw(vec_p) < 0) return false;
    }
    memcpy(
        (byte_t*)vec_p->vec_buf_ptr + vec_p->vec_elem_size * vec_p->vec_len,
        itm_p,
        vec_p->vec_elem_size
    );
    ++vec_p->vec_len;
    return true;
}

bool Vector_pop_raw(Vector *vec_p) {
    if (!vec_p || !vec_p->vec_buf_ptr || !vec_p->vec_len) return false;
    --vec_p->vec_len;
    return true;
}

void *Vector_at_raw(Vector *vec_p, size_t idx) {
    if (
        !vec_p ||
        !vec_p->vec_buf_ptr ||
        !vec_p->vec_len ||
        idx >= vec_p->vec_len
    )
        return NULL;
    return (byte_t*)vec_p->vec_buf_ptr + vec_p->vec_elem_size * idx;
}

void *Vector_begin_raw(Vector *vec_p) {
    return vec_p->vec_buf_ptr;
}

void *Vector_end_raw(Vector *vec_p) {
    return (byte_t*)vec_p->vec_buf_ptr + vec_p->vec_elem_size * vec_p->vec_len;
}

static struct _c_vector_ops vec_vtbl = {
    .at     = &Vector_at_raw,
    .begin  = &Vector_begin_raw,
    .end    = &Vector_end_raw,
    .push   = &Vector_push_raw,
    .pop    = &Vector_pop_raw,
};

void Vector_destroy(Vector *vec_p) {
    if (!vec_p) return;
    if (vec_p->vec_buf_ptr) free(vec_p->vec_buf_ptr);
    free(vec_p);
    return;
}

static inline Vector *_vector_intrnl_dtor(Vector *vec_p) {
    Vector_destroy(vec_p);
    return NULL;
}

Vector *Vector_create(
    size_t vec_elem_size,
    size_t vec_size
) {
    if (!vec_elem_size || !vec_size) return NULL;
    Vector *vec_p = (Vector*)malloc(sizeof(Vector));
    if (!vec_p) return _vector_intrnl_dtor(vec_p);
    *vec_p = (Vector) {0};

    vec_p->vec_buf_ptr = malloc(vec_size * vec_elem_size);
    if (!vec_p->vec_buf_ptr) return _vector_intrnl_dtor(vec_p);

    vec_p->vec_max_elem_cnt = vec_size;
    vec_p->vec_len = 0;
    vec_p->vec_elem_size = vec_elem_size;
    vec_p->vec_vtbl_p = &vec_vtbl;

//  logic
    return vec_p;
}

