#pragma once

#include <global_intdef.h>
#include <global_ptrdef.h>
#include <compute_file_crc32.h>
#include <unistd.h>

#define PAGE_SIZE   4096
#define PAD_SIZE    16
#define MAGIC_LEN   sizeof(qword_t)
#define VERSION     ((word_t)0x0001)
#define ENDIANESS   0U   // LE

#define FLAG_NONE   0U

#define DEFAULT_INDEX_PAGE_IDX 0

#define PAD_OFF_ALIGN(off, pad_size) ((off + pad_size - 1) &~ (pad_size - 1))

static const byte_t PAGE_FILE_MAGIC[MAGIC_LEN] = { 0xff, 'P', 'A', 'G', 'E', 0x1a, '\n', 0x01 };
static const byte_t PAGE_MAGIC[MAGIC_LEN]      = { '\n', '.', 'P', 'A', 'G', 'E', '\n', 0x02 };
static const byte_t PAGE_END_MAGIC[MAGIC_LEN]      = { 0x03, '.', 'E', 'N', 'D', 'P', 'G', '\n' };
// static const byte_t PAGE_EOF_MAGIC[MAGIC_LEN]  = { '\n', '.', 'E', 'O', 'F', '\n', 0x04, '\0' };

enum PAGE_FILE_OBJECT_ERRNO {
    PAGE_INVALID_PGFOBJ = -1,
    PAGE_INVALID_MEMBUF = -2,
    PAGE_INVALID_FILDES = -3,
    PAGE_INVALID_PGNO   = -4,
    PAGE_INVALID_PGCUR  = -5,
    PAGE_NULL_BUFFER    = -6,
    PAGE_SIZE_NOT_POWER_OF_2 = -7,
};

static inline strlit_t page_strerror(int pfo_errno) {
    strlit_t _errmsg = NULL_STRING;
    switch (pfo_errno) {
        case (PAGE_INVALID_PGFOBJ):
            _errmsg = "invalid page file object";
            break;
        case (PAGE_INVALID_MEMBUF):
            _errmsg = "invalid page mem buffer";
            break;
        case (PAGE_INVALID_FILDES):
            _errmsg = "invalid page file descriptor";
            break;
        case (PAGE_INVALID_PGNO):
            _errmsg = "invalid page index";
            break;
        case (PAGE_INVALID_PGCUR):
            _errmsg = "invalid page cur";
            break;
        case (PAGE_NULL_BUFFER):
            _errmsg = "null buffer";
            break;
        case (PAGE_SIZE_NOT_POWER_OF_2):
            _errmsg = "page size is not power of 2";
            break;
        default:
            _errmsg = "no error";
            break;
    }
    return _errmsg;
}


struct _page_file_header_inner {
    qword_t     pfh_magic;          // [0]
    word_t      pfh_version;        // [1]
    word_t      pfh_flags;          // [2]
    word_t      pfh_size;           // [3]
    word_t      pfh_pad_size;       // [4]
    dword_t     pfh_page_size;      // [5]
    dword_t     pfh_page_count;     // [6]
    dword_t     pfh_index_page_idx; // [7]
    uoff64_t    pfh_first_page_off; // [8]
    uoff64_t    pfh_eof_off;        // [9]
} __attribute__((packed));

static const size_t PAGE_FILE_HEADER_PAD_SIZE =
    PAD_OFF_ALIGN(
        sizeof(struct _page_file_header_inner) + sizeof(dword_t),
        PAD_SIZE
    ) - (sizeof(struct _page_file_header_inner) + sizeof(dword_t));

typedef struct _page_file_header {
    struct _page_file_header_inner pfh_inner;
    dword_t     pfh_crc32;
#if PAGE_FILE_HEADER_PAD_SIZE
    byte_t      reserved[PAGE_FILE_HEADER_PAD_SIZE];
#endif
} __attribute__((packed)) PAGE_FILE_HEADER;

struct _page_header_inner {
    qword_t     ph_magic;       // [0]
    word_t      ph_flags;       // [1]
    size32_t    ph_size;        // [2]
    dword_t     ph_page_idx;    // [3]
    uoff32_t    ph_page_cur;    // [4]      // NEW
    qword_t     ph_footer_off;  // [5]
} __attribute__((packed));

static const size_t PAGE_HEADER_PAD_SIZE =
    PAD_OFF_ALIGN(
        sizeof(struct _page_header_inner) + sizeof(dword_t),
        PAD_SIZE
    ) - (sizeof(struct _page_header_inner) + sizeof(dword_t));

typedef struct _page_header {
    struct _page_header_inner ph_inner;
    dword_t     ph_crc32;
#if PAGE_HEADER_PAD_SIZE
    byte_t      reserved[PAGE_HEADER_PAD_SIZE];
#endif
} __attribute__((packed)) PAGE_HEADER;

typedef struct _page_footer {
    dword_t reserved;
    dword_t pf_crc32;
    qword_t pf_magic;
} __attribute__((packed)) PAGE_FOOTER;


typedef struct _page_file_footer {
    dword_t reserved;
    dword_t pff_crc32;
    qword_t pff_magic;
} __attribute__((packed)) PAGE_FILE_FOOTER;


typedef struct _page_file_obj {
    int         pfo_file_desc;  // [0]
    void       *pfo_file_mem;   // [1]
    size64_t    pfo_file_size;  // [2]
    size32_t    pfo_page_cnt;   // [3]
    uoff64_t    pfo_file_cur;   // [4]
    long_t      pfo_page_idx;   // [5]
    off32_t     pfo_page_cur;   // [6]

    int         pfo_errno;      // [7]
} PAGE_FILE_OBJECT;

int PAGE_FILE_OBJECT_init(
    PAGE_FILE_OBJECT   *_this,
    int                 page_file_desc,
    void               *page_file_mem,
    size64_t            page_file_size,
    ulong_t             page_count
);
int PAGE_FILE_goto_page(
    PAGE_FILE_OBJECT   *_this,
    ulong_t             page_idx
);
int PAGE_FILE_page_read_buffer_raw(
    PAGE_FILE_OBJECT   *_this,
    ulong_t             page_idx,
    uoff32_t            page_cur,
    size_t              buf_len,
    byte_t             *out_buf_p
);
int PAGE_FILE_page_write_buffer_raw(
    PAGE_FILE_OBJECT   *_this,
    ulong_t             page_idx,
    uoff32_t            page_cur,
    byte_t              buf[],
    size_t              buf_len
);
void PAGE_FILE_OBJECT_deinit(
    PAGE_FILE_OBJECT   *_this
);




static inline PAGE_FILE_HEADER *page_get_fileheader_ptr(addr_t base_addr) {
    return (PAGE_FILE_HEADER*)base_addr;
}

static inline PAGE_HEADER *page_get_pageheader_ptr(
    addr_t  base_addr,
    dword_t page_idx
) {
    if (!base_addr) return NULL;
    return (PAGE_HEADER*)(base_addr + page_idx * PAGE_SIZE);
}

static inline PAGE_FOOTER *page_get_pagefooter_ptr(
    addr_t  base_addr,
    dword_t page_idx
) {
    if (!base_addr) return NULL;
    return (PAGE_FOOTER*)(
        base_addr + (page_idx + 1) * PAGE_SIZE - sizeof(PAGE_FOOTER)
    );
}

static inline void page_fill_fileheader(
    PAGE_FILE_HEADER   *pf_hdr_p,
    word_t              pfh_flags,
    size32_t            pfh_page_count,
    dword_t             pfh_index_page_idx
) {
    pf_hdr_p->pfh_inner.pfh_magic           = *(qword_t*)PAGE_FILE_MAGIC;
    pf_hdr_p->pfh_inner.pfh_version         = VERSION;
    pf_hdr_p->pfh_inner.pfh_flags           = pfh_flags;
    pf_hdr_p->pfh_inner.pfh_size            = sizeof(PAGE_FILE_HEADER);
    pf_hdr_p->pfh_inner.pfh_pad_size        = PAD_SIZE;
    pf_hdr_p->pfh_inner.pfh_page_size       = PAGE_SIZE;
    pf_hdr_p->pfh_inner.pfh_page_count      = pfh_page_count;
    pf_hdr_p->pfh_inner.pfh_index_page_idx  = pfh_index_page_idx;
    pf_hdr_p->pfh_inner.pfh_first_page_off  = 0;
    pf_hdr_p->pfh_inner.pfh_eof_off         = pfh_page_count * PAGE_SIZE;
    pf_hdr_p->pfh_crc32                     = compute_mem_crc32(&pf_hdr_p->pfh_inner, sizeof(pf_hdr_p->pfh_inner));
}

static inline void page_fill_pageheader(
    addr_t  base_addr,
    word_t  page_flags,
    dword_t page_idx
) {
    PAGE_HEADER *pg_hdr_p = page_get_pageheader_ptr(base_addr, page_idx);
    pg_hdr_p->ph_inner.ph_magic         = *(qword_t*)PAGE_MAGIC;
    pg_hdr_p->ph_inner.ph_flags         = page_flags;
    pg_hdr_p->ph_inner.ph_size          = sizeof(PAGE_HEADER);
    pg_hdr_p->ph_inner.ph_page_idx      = page_idx;
    pg_hdr_p->ph_inner.ph_page_cur      = sizeof(PAGE_HEADER);
    pg_hdr_p->ph_inner.ph_footer_off    = PAGE_SIZE - sizeof(PAGE_FOOTER);
    pg_hdr_p->ph_crc32                  = compute_mem_crc32(&pg_hdr_p->ph_inner, sizeof(pg_hdr_p->ph_inner));
}

static inline void page_fill_pagefooter(
    addr_t  base_addr,
    dword_t page_idx
) {
    uoff64_t page_off = page_idx * PAGE_SIZE;
    ptr_t page_p = (ptr_t)((byte_ptr_t)base_addr + page_off);
    PAGE_FOOTER *pg_ftr_p = page_get_pagefooter_ptr(base_addr, page_idx);
    pg_ftr_p->pf_crc32  = compute_mem_crc32(page_p, PAGE_SIZE - sizeof(PAGE_FOOTER));
    pg_ftr_p->pf_magic  = *(qword_t*)PAGE_END_MAGIC;
}

static inline int PAGE_FILE_page_read(
    PAGE_FILE_OBJECT   *_this,
    size_t              buf_len,
    byte_t             *out_buf_p
) {
    int ret = PAGE_FILE_page_read_buffer_raw(
        _this,
        _this->pfo_page_idx,
        _this->pfo_page_cur,
        buf_len,
        out_buf_p
    );
    if (ret < 0) return ret;
    _this->pfo_page_cur += buf_len;
    page_get_pageheader_ptr(
        (addr_t)_this->pfo_file_mem, _this->pfo_page_idx
    )->ph_inner.ph_page_cur += buf_len;
    return buf_len;
}
static inline int PAGE_FILE_page_write(
    PAGE_FILE_OBJECT   *_this,
    byte_t              buf[],
    size_t              buf_len
) {
    int ret = PAGE_FILE_page_write_buffer_raw(
        _this,
        _this->pfo_page_idx,
        _this->pfo_page_cur,
        buf,
        buf_len
    );
    if (ret < 0) return ret;
    _this->pfo_page_cur += buf_len;
    page_get_pageheader_ptr(
        (addr_t)_this->pfo_file_mem, _this->pfo_page_idx
    )->ph_inner.ph_page_cur += buf_len;
    return buf_len;
}