#ifndef __C_ALGO_H__
#define __C_ALGO_H__

#include <stddef.h>

#ifndef __cplusplus
#define static_assert _Static_assert
#endif /*__cplusplus*/

#define insertion_sort(arr, arr_len) \
    do {\
        if (!arr || !arr_len) break;\
        for (size_t i = 1; i < arr_len; ++i) {\
            __typeof__(arr[0]) key = arr[i];\
            size_t j = i;\
            while (j > 0 && arr[j - 1] > key) {\
                arr[j] = arr[j - 1];\
                --j;\
            }\
            arr[j] = key;\
        }\
    } while(0)


#define binary_search(arr, arr_len, key) ({\
    int idx = -1;\
    static_assert(\
        __builtin_types_compatible_p(__typeof__(arr[0]), __typeof__(key)),\
        "Vector_push: key type mismatch"\
    );\
    do {\
        if (!arr || !arr_len) break;\
        size_t begin_idx = 0;\
        size_t end_idx = arr_len - 1;\
        while (begin_idx <= end_idx) {\
            int pivot_idx = begin_idx + (end_idx - begin_idx) / 2;\
            if (arr[pivot_idx] == key) {\
                idx = pivot_idx;\
                break;\
            }\
            if (arr[pivot_idx] < key) {\
                begin_idx = pivot_idx + 1;\
            } else {\
                end_idx = pivot_idx - 1;\
            }\
        }\
    } while(0);\
    idx;\
})




#endif /*__C_ALGO_H__*/
