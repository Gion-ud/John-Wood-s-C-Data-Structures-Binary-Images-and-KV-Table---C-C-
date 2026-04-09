#pragma once

#include "kvtable_priv.h"
#include "global_intdef.h"
#include <align.h>

#define KV_FILE_ALIGN   4
#define KV_FILE_VERSION ((word_t)0x0201)
#define KV_FILE_MAGIC   ((dword_t)0xBBA9B423)
#define KV_EOF_MAGIC    ((dword_t)0x04037F1A)

typedef enum _kv_file_flags {
    KVFILE_NOFLAGS      = 0U,
    KVFILE_LE           = (1U << 0),
    KVFILE_BE           = (1U << 1),
    KVFILE_ALIGNED      = (1U << 2),
    KVFILE_COMPRESSED   = (1U << 3),
    KVFILE_ENCRYPTED    = (1U << 4),
} KvFileFlags;

#define KVFILE_DEFAULT_FLAGS (KVFILE_LE | KVFILE_ALIGNED)

#define ALIGNED __attribute__((aligned(KV_FILE_ALIGN)))
#define PACKED __attribute__((packed))

typedef struct _kv_fileheader {
    dword_t fh_magic;       // [0]
    word_t  fh_version;     // [1]
    word_t  fh_align;       // [2]
    word_t  fh_fhdrsize;    // [3]
    word_t  fh_fftrsize;    // [4]
    dword_t fh_flags;       // [5]
    dword_t fh_kvcnt;       // [6]
    dword_t fh_tocoff;      // [7]
    dword_t fh_kvdatoff;    // [8]
    dword_t fh_fftroff;     // [9]
} PACKED KvFileHeader; 

typedef struct _dat_filefooter {
    dword_t crc32;          // [0]
    dword_t ff_magic;       // [1]
} PACKED KvFileFooter;

typedef struct _kv_file_entryheader {
    word_t  flags;          // [0]
    word_t  key_len;        // [1]
    dword_t key_off;        // [2]
    dword_t val_len;        // [3]
    dword_t val_off;        // [4]
} PACKED KvFileEntry;       // offsets are relative


