#ifndef __C_ALGO_H__
#define __C_ALGO_H__

#include <stddef.h>

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
    } while(0);



#endif
