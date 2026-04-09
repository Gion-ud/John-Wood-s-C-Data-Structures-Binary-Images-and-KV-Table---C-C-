#ifndef GLOBAL_INTDEF_H
#define GLOBAL_INTDEF_H

#include <stddef.h>
#include <stdint.h>

#define BYTE_SIZE   1U
#define WORD_SIZE   2U
#define DWORD_SIZE  4U
#define QWORD_SIZE  8U
#define XWORD_SIZE  16U


typedef unsigned char       byte_t;
typedef unsigned short      word_t;
typedef unsigned int        dword_t;
typedef unsigned long long  qword_t;

typedef signed char         char_t;
typedef signed short        short_t;
typedef signed int          long_t;
typedef signed long long    longlong_t;

typedef unsigned char       uchar_t;
typedef unsigned short      ushort_t;
typedef unsigned int        ulong_t;
typedef unsigned long long  ulonglong_t;

typedef long_t              ssize32_t;
typedef longlong_t          ssize64_t;
typedef ulong_t             size32_t;
typedef ulonglong_t         size64_t;

#include <sys/types.h>

#ifndef __MINGW32__
typedef long_t              off32_t;
typedef longlong_t          off64_t;
#endif
typedef ulong_t             uoff32_t;
typedef ulonglong_t         uoff64_t;

typedef ulong_t             addr32_t;
typedef ulonglong_t         addr64_t;
typedef uintptr_t           addr_t;

typedef byte_t              xword_t[XWORD_SIZE];

#ifndef __cplusplus
#ifndef static_assert
#define static_assert _Static_assert
#endif
#endif

static_assert(sizeof(byte_t)   == 1, "byte_t must be 1 byte");
static_assert(sizeof(word_t)   == 2, "word_t must be 2 bytes");
static_assert(sizeof(dword_t)  == 4, "dword_t must be 4 bytes");
static_assert(sizeof(qword_t)  == 8, "qword_t must be 8 bytes");
static_assert(sizeof(xword_t)  == 16, "xword_t must be 16 bytes");

static_assert(sizeof(char_t)       == 1, "char_t must be 1 byte");
static_assert(sizeof(short_t)      == 2, "short_t must be 2 bytes");
static_assert(sizeof(long_t)       == 4, "long_t must be 4 bytes");
static_assert(sizeof(longlong_t)   == 8, "longlong_t must be 8 bytes");

static_assert(sizeof(uchar_t)      == 1, "uchar_t must be 1 byte");
static_assert(sizeof(ushort_t)     == 2, "ushort_t must be 2 bytes");
static_assert(sizeof(ulong_t)      == 4, "ulong_t must be 4 bytes");
static_assert(sizeof(ulonglong_t)  == 8, "ulonglong_t must be 8 bytes");


#endif