#ifndef KV_DEF_H
#define KV_DEF_H

#include <tlv/tlv.h>
#include <global_intdef.h>
#include <typeflags.h>

#include "hash_def.h"


/*
enum data_types {
    DATA_TYPE_SZ  = 0,
    DATA_TYPE_LP  = 1,
    DATA_TYPE_TLV = 2,
    DATA_TYPE_XWORD_INLINED = 3,
};
typedef struct data_typed {
    dword_t data_type;
    union data {
        byte_t      data_byte_inlined;
        word_t      data_word_inlined;
        dword_t     data_dword_inlined;
        qword_t     data_qword_inlined;
        xword_t     data_xword_inlined;
        LPBuffer    data_lpbuf;
    } data;
} TypedData;

typedef struct _kv_entry {
    size_t      key_len;
    size_t      key_off;
    size_t      val_len;
    size_t      val_off;
//  void       *key_data;
//  void       *val_data;
    dword_t     key_hash;
    int         entry_stat;
} KVEntry;
*/

typedef struct _pub_kv_pair {
    size_t  key_len;
    void   *key_ptr;
    size_t  val_len;
    void   *val_ptr;
} KVPairView;




#endif