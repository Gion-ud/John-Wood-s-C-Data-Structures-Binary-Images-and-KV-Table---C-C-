#ifndef GLOBAL_H
#define GLOBAL_H

// Explicitly ban BE
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    #error "BE systems are NOT supported"
#elif defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN) || defined(__ARMEB__)
    #error "BE systems are NOT supported"
#endif


#define LOOP_RUNNING    1
#define BUFFER_SIZE     4096
#define FILE_PATH_LEN   256



#endif