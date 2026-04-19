#ifndef GLOBAL_INTDEF_H
#define GLOBAL_INTDEF_H

#include <stddef.h>
#include <stdint.h>

#define BYTE_SIZE   1U
#define WORD_SIZE   2U
#define DWORD_SIZE  4U
#define QWORD_SIZE  8U

typedef unsigned char   byte_t;
typedef uint16_t        word_t;
typedef uint32_t        dword_t;
typedef uint64_t        qword_t;

typedef signed char         char_t;
typedef signed short        short_t;
typedef signed int          int_t;
typedef signed long         long_t;
typedef signed long long    longlong_t;

typedef unsigned char       uchar_t;
typedef unsigned short      ushort_t;
typedef unsigned int        uint_t;
typedef unsigned long       ulong_t;
typedef unsigned long long  ulonglong_t;

typedef int32_t     ssize32_t;
typedef int64_t     ssize64_t;
typedef uint32_t    size32_t;
typedef uint64_t    size64_t;

#include <sys/types.h>

#ifndef __MINGW32__
typedef int32_t     off32_t;
typedef int64_t     off64_t;
#endif
typedef uint32_t    uoff32_t;
typedef uint64_t    uoff64_t;

typedef uint32_t    addr32_t;
typedef uint64_t    addr64_t;
typedef uintptr_t   addr_t;

typedef ulong_t     uoff_t;
typedef longlong_t  loff_t;




#endif