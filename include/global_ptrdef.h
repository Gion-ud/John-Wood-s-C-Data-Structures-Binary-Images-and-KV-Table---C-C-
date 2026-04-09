#ifndef GLOBAL_PTRDEF_H
#define GLOBAL_PTRDEF_H

#include "global_intdef.h"

#define EMPTY_STRING    ""
#define NULL_STRING     ((char*)0)

typedef void       *ptr_t;
typedef void       *void_ptr_t;
typedef byte_t     *byte_ptr_t;
typedef word_t     *word_ptr_t;
typedef dword_t    *dword_ptr_t;
typedef qword_t    *qword_ptr_t;

typedef const char *strlit_t;


#endif