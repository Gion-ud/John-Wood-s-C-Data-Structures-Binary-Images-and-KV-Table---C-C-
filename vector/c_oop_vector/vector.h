#ifndef __C_VECTOR_H__
#define __C_VECTOR_H__

#include <stddef.h>
#include <stdbool.h>


typedef struct _c_vector Vector;

struct _c_vector_ops {
    void   *(*at)(Vector *_this, size_t idx);
    void   *(*begin)(Vector *_this);
    void   *(*end)(Vector *_this);
    bool    (*push)(Vector *_this, void *elem_p);
    bool    (*pop)(Vector *_this);
};

struct _c_vector {
    void   *vec_buf_ptr;
    size_t  vec_elem_size;
    size_t  vec_len;
    size_t  vec_max_elem_cnt;
    struct _c_vector_ops *vec_vtbl_p;
};


/*
static struct _c_vector_ops vec_vtbl = {
    .at     = &Vector_at_raw,
    .begin  = &Vector_begin_raw,
    .end    = &Vector_end_raw,
    .push   = &Vector_push_raw,
    .pop    = &Vector_pop_raw,
};

extern bool     Vector_push_raw(Vector *vec_p, void *itm_p);
extern bool     Vector_pop_raw(Vector *vec_p);
extern void    *Vector_at_raw(Vector *vec_p, size_t idx);
extern void    *Vector_begin_raw(Vector *vec_p);
extern void    *Vector_end_raw(Vector *vec_p);
*/

extern Vector  *Vector_create(size_t vec_elem_size, size_t vec_size);
extern void     Vector_destroy(Vector *vec_p);

#define new_Vector(Type, vec_size) ({   \
    Vector *vec_p = NULL;               \
    do {                                \
        vec_p = Vector_create(sizeof(Type), (vec_size));\
        if (!(vec_p)) break;            \
    } while(0);                         \
    vec_p;                              \
})

#define delete_Vector(vec_p) Vector_destroy(vec_p)

#define Vector_push(Type, _this, itm) ({\
    bool st = false;\
    do {\
        if (!(_this) || (_this)->vec_elem_size != sizeof(Type)\
        ) break;\
        Type tmp_itm = (itm);\
        st = (_this)->vec_vtbl_p->push(_this, &tmp_itm);\
    } while(0);\
    st;\
})

#define Vector_pop(_this) ({\
    bool st = false;\
    do {\
        if (!(_this)) break;\
        st = (_this)->vec_vtbl_p->pop(_this);\
    } while(0);\
    st;\
})

#define Vector_at(_this, Type, idx) ({\
    Type *itm_p = (Type*)0;\
    do {\
        if (!(_this) || (_this)->vec_elem_size != sizeof(Type)) break;\
        itm_p = (_this)->vec_vtbl_p->at(_this, idx);\
    } while(0);\
    itm_p;\
})

#define Vector_begin(_this, Type) ((Type*)(_this)->vec_vtbl_p->begin(_this))
#define Vector_end(_this, Type) ((Type*)(_this)->vec_vtbl_p->end(_this))




#endif /*__C_VECTOR_H__*/