#ifndef __C_VECTOR_H__
#define __C_VECTOR_H__

#include "vecraw.h"
#include <stdlib.h>
#include <assert.h>

typedef _c_Vector_core Vector;

#define Vector_init(T, vec_p) ({\
    bool __success = false;\
    do {\
        if ((vec_p) == NULL) break;\
        ssize_t __ret = _c_vector_init(\
            vec_p,\
            sizeof(T),\
            C_VECTOR_MIN_DEFAULT_CAPACITY\
        );\
        if (__ret < 0) break;\
        __success = true;\
    } while(0);\
    __success;\
})

#define Vector_deinit(vec_p) \
    do {\
        if ((vec_p) == NULL) break;\
        _c_vector_deinit(vec_p);\
    } while(0)

#define new_Vector(T) ({\
    Vector *__vec_p = NULL;\
    do {\
        __vec_p = _c_vector_create(sizeof(T), C_VECTOR_MIN_DEFAULT_CAPACITY);\
        if (!__vec_p) break;\
    } while(0);\
    __vec_p;\
})

#define delete_Vector(vec_p) \
    do {\
        if (!vec_p) break;\
        _c_vector_destroy(vec_p);\
    } while(0)


#define Vector_init(T, vec_p) ({\
    bool __success = false;\
    do {\
        if ((vec_p) == NULL) break;\
        ssize_t __ret = _c_vector_init(\
            vec_p,\
            sizeof(T),\
            C_VECTOR_MIN_DEFAULT_CAPACITY\
        );\
        if (__ret < 0) break;\
        __success = true;\
    } while(0);\
    __success;\
})


#define Vector_init_assign_literal(T, vec_p, ...) ({\
    bool __success = false;\
    do {\
        T __tmp_arr[] = __VA_ARGS__;\
        const size_t elem_arr_len = sizeof(__tmp_arr) / sizeof(T);\
        ssize_t __ret = _c_vector_init_assign(\
            vec_p,\
            sizeof(T),\
            elem_arr_len,\
            (T*)__tmp_arr\
        );\
        if (__ret < 0) break;\
        __success = true;\
    } while(0);\
    __success;\
})
#define new_Vector_assign_literal(T, ...) ({\
    Vector *__vec_p = NULL;\
    do {\
        T __tmp_arr[] = __VA_ARGS__;\
        const size_t elem_arr_len = sizeof(__tmp_arr) / sizeof(T);\
        __vec_p = _c_vector_create_assign(\
            sizeof(T),\
            elem_arr_len,\
            (T*)__tmp_arr\
        );\
    } while(0);\
    __vec_p;\
})




#define Vector_push(T, vec_p, itm_ref_p) ({\
    bool __success = false;\
    do {\
        if (!vec_p) break;\
        _Static_assert(\
            __builtin_types_compatible_p(T*, __typeof__(itm_ref_p)),\
            "Vector_push: elem type mismatch"\
        );\
        assert(sizeof(T) == (vec_p)->v_elem_size);\
        if (_c_vector_push(vec_p, itm_ref_p) < 0) break;\
        __success = true;\
    } while(0);\
    __success;\
})

#define Vector_pop(vec_p) ({\
    bool __success = false;\
    do {\
        if (!vec_p) break;\
        if (_c_vector_pop(vec_p) < 0) break;\
        __success = true;\
    } while(0);\
    __success;\
})

#define Vector_get(T, vec_p, idx) ({\
    T *__itm_p = NULL;\
    do {\
        if (!vec_p) break;\
        assert(sizeof(T) == (vec_p)->v_elem_size);\
        __itm_p = (T*)_c_vector_get(vec_p, idx);\
    } while(0);\
    __itm_p;\
})

#define Vector_data(T, vec_p) ({\
    T *__data_p = NULL;\
    do {\
        if (!vec_p) break;\
        assert(sizeof(T) == (vec_p)->v_elem_size);\
        __data_p = (T*)_c_vector_data(vec_p);\
    } while(0);\
    __data_p;\
})
#define Vector_begin(T, vec_p) ({\
    T *__begin_p = NULL;\
    do {\
        if (!vec_p) break;\
        assert(sizeof(T) == (vec_p)->v_elem_size);\
        __begin_p = (T*)_c_vector_begin(vec_p);\
    } while(0);\
    __begin_p;\
})
#define Vector_end(T, vec_p) ({\
    T *__end_p = NULL;\
    do {\
        if (!vec_p) break;\
        assert(sizeof(T) == (vec_p)->v_elem_size);\
        __end_p = (T*)_c_vector_end(vec_p);\
    } while(0);\
    __end_p;\
})

#define Vector_size(vec_p) ({\
    size_t size = 0;\
    do {\
        if (!vec_p) break;\
        ssize_t ret = _c_vector_size(vec_p);\
        if (ret < 0) break;\
        size = ret;\
    } while(0);\
    size;\
})
#define Vector_capacity(vec_p) ({\
    size_t cap = 0;\
    do {\
        if (!vec_p) break;\
        ssize_t ret = _c_vector_capacity(vec_p);\
        if (ret < 0) break;\
        cap = ret;\
    } while(0);\
    cap;\
})
#define Vector_empty(vec_p) ({\
    bool b = false;\
    do {\
        if (!vec_p) break;\
        b = _c_vector_empty(vec_p);\
    } while(0);\
    b;\
})

#include <stdio.h>
#define Vector_reserve(vec_p, size) ({\
    bool __success = false;\
    do {\
        if (!vec_p) break;\
        ssize_t __ret = _c_vector_reserve(vec_p, size);\
        if (__ret < 0) break;\
        __success = true;\
    } while(0);\
    __success;\
})
#define Vector_clear(vec_p) \
    do {\
        if (!vec_p) break;\
        _c_vector_clear(vec_p);\
    } while(0);\




#endif /*__C_VECTOR_H__*/