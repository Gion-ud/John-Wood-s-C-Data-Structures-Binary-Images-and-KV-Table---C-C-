// kvdb.h
#ifndef TLV_H
#define TLV_H

#include "global_intdef.h"

typedef struct _tlv_obj {
    dword_t     type;
    size_t      len;
    void       *data;
} TLVBuffer;

typedef struct _lp_buf {
    size_t      len;
    void       *data;
} LPBuffer;


#endif