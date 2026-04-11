// vector.h
#ifndef __C_VECTOR_H__
#define __C_VECTOR_H__

#include <stddef.h>

#define MIN_VECTOR_CAP 1
#define INLINED static inline

/*
struct c_array_header {
    size_t elem_size;
    size_t length;
};
typedef struct _c_array {
    struct c_array_header vec_hdr;
    void *arr_p;
} Array;
typedef void *HPDSArrayHandle;
*/


struct c_vector_header {
    size_t elem_size;
    size_t length;
    size_t capacity;
};

typedef struct _c_vector {
    struct c_vector_header vec_hdr;
    void *vec_p;
} Vector;

typedef void *HPDSVectorHandle;

extern HPDSVectorHandle HPDSVector_create(size_t elem_size);
extern int HPDSVector_push(HPDSVectorHandle *vec_hp, void *elem_p);
extern void HPDSVector_destroy(HPDSVectorHandle vec_h);

INLINED struct c_vector_header *_HPDSVector_header(HPDSVectorHandle vec_h) {
    return (struct c_vector_header*)vec_h - 1;
}
INLINED void *_HPDSVector_head(HPDSVectorHandle vec_h) {
    return (void*)_HPDSVector_header(vec_h);
}
INLINED void *HPDSVector_begin(HPDSVectorHandle vec_h) {
    return (void*)vec_h;
}
INLINED void *HPDSVector_end(HPDSVectorHandle vec_h) {
    struct c_vector_header *vec_hdr_p = _HPDSVector_header(vec_h);
    return (unsigned char*)vec_h + vec_hdr_p->length * vec_hdr_p->elem_size;
}
INLINED void *HPDSVector_get(HPDSVectorHandle vec_h, size_t idx) { // No Bound Check
    if (!vec_h) return NULL;
    return (unsigned char*)vec_h + idx * _HPDSVector_header(vec_h)->elem_size;
}

#define Vector(T) T*

#define Vector_create(T) ({\
    T *__vec_p = NULL;\
    do {\
        __vec_p = HPDSVector_create(sizeof(T));\
    } while(0);\
    __vec_p;\
})
#define Vector_destroy(vec_p) HPDSVector_destroy(vec_p)
#define Vector_push(vec_p, elem) ({\
    int __ret = 0;\
    _Static_assert(\
        __builtin_types_compatible_p(__typeof__(elem), __typeof__(*vec_p)),\
        "Vector_push: elem type mismatch"\
    );\
    do {\
        if (!vec_p) break;\
        __typeof__(elem) __elem = elem;\
        __ret = HPDSVector_push((HPDSVectorHandle*)&vec_p, &__elem);\
    } while(0);\
    __ret;\
})
#define Vector_length(vec_p) ({\
    size_t __len = 0;\
    struct c_vector_header *vec_hdr_p = NULL;\
    do {\
        if (!vec_p) break;\
        vec_hdr_p = _HPDSVector_header(vec_p);\
        __len = vec_hdr_p->length;\
    } while(0);\
    __len;\
})
#define Vector_begin(vec_p) ({\
    __typeof__(vec_p) __begin = (__typeof__(vec_p))HPDSVector_begin((HPDSVectorHandle)vec_p);\
    __begin;\
})
#define Vector_end(vec_p) ({\
    __typeof__(vec_p) __end = NULL;\
    do {\
        if (!vec_p) break;\
        __end = (__typeof__(vec_p))HPDSVector_end((HPDSVectorHandle)vec_p);\
    } while(0);\
    __end;\
})




//#define _C_VECTOR_IMPLM

#ifdef _C_VECTOR_IMPLM
#include <stdlib.h>
#include <string.h>
HPDSVectorHandle HPDSVector_create(size_t elem_size) {
    void *mem = malloc(sizeof(struct c_vector_header) + MIN_VECTOR_CAP * elem_size);
    if (!mem) return NULL;
    *(struct c_vector_header*)mem = (struct c_vector_header) {
        .elem_size = elem_size,
        .length = 0,
        .capacity = MIN_VECTOR_CAP,
    };
    return (HPDSVectorHandle)((unsigned char*)mem + sizeof(struct c_vector_header));
}
int HPDSVector_push(HPDSVectorHandle *vec_hp, void *elem_p) {
    if (!vec_hp || !*vec_hp || !elem_p) return -1;
    struct c_vector_header *vec_hdr_p = _HPDSVector_header(*vec_hp);
    if (vec_hdr_p->length >= vec_hdr_p->capacity) {
        size_t new_capacity = vec_hdr_p->capacity * 2;
        struct c_vector_header *vec_hdr_tmp_p = (struct c_vector_header *)realloc(
            (unsigned char*)vec_hdr_p,
            sizeof(struct c_vector_header) + new_capacity * vec_hdr_p->elem_size
        );
        if (!vec_hdr_tmp_p) return -1;
        vec_hdr_p = vec_hdr_tmp_p;
        *vec_hp = (unsigned char*)vec_hdr_tmp_p + sizeof(struct c_vector_header);
        vec_hdr_p->capacity = new_capacity;
    }
    memcpy(
        (unsigned char*)*vec_hp + vec_hdr_p->length * vec_hdr_p->elem_size,
        elem_p,
        vec_hdr_p->elem_size
    );
    ++vec_hdr_p->length;
    return 0;
}
void HPDSVector_destroy(HPDSVectorHandle vec_h) {
    if (!vec_h) return;
    free(_HPDSVector_head(vec_h));
}
#endif /* _C_VECTOR_IMPLM */



#endif /*__C_VECTOR_H__*/