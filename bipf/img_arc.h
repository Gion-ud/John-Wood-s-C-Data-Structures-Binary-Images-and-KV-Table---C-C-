#ifndef BINARY_IMAGE_H
#define BINARY_IMAGE_H

#include <global_intdef.h>
#include <global_ptrdef.h>
#include <compute_crc32.h>
#include <posix_io.h>
#include <unistd.h>
#include <stdbool.h>

#define ALIGN_SIZE  16
#define MAGIC_LEN   sizeof(qword_t)
#define VERSION     ((word_t)0x0101)
#define ENDIANESS   0U   // LE

#define FLAG_NONE   0U

#define ALIGN_OFFSET(off, align_size) ((off + align_size - 1) &~ (align_size - 1))
static inline bool _OFF_IS_POWER_OF_2(qword_t off) {
    if ((off & (off - 1)) != 0) return false;
    return true;
}


static const byte_t IMG_FILE_MAGIC[MAGIC_LEN] = { 0xff, 'I', 'M', 'A', 'G', 'E', '\n', 0x00 };
// static const byte_t PAGE_EOF_MAGIC[MAGIC_LEN]  = { '\n', '.', 'E', 'O', 'F', '\n', 0x04, '\0' };

enum IMG_FILE_OBJECT_ERRNO {
    IMGERR_INVALID_IFOBJ   = -1,
    IMGERR_INVALID_MEMBUF  = -2,
    IMGERR_INVALID_FILDES  = -3,
    IMGERR_INVALID_ICUR    = -5,
    IMGERR_NULL_BUF        = -6,
    IMGERR_NULL_PTR        = -7,
    IMGERR_WRITE_FAILED    = -8,
    IMGERR_FILE_FULL       = -9,
    IMGERR_CALC_CRC_FAILED = -10,
    IMGERR_INVALID_CRC     = -11,
    IMGERR_INVALID_ENTRY_COUNT = -12,
};

enum IMG_ENTRY_TYPES {
    IMG_ENTRY_ROOT  = 0,
    IMG_ENTRY_FILE  = 1,
    IMG_ENTRY_DIR   = 2,
    IMG_ENTRY_LINK  = 3,
};

struct _img_error {
    int         img_errno;
    strlit_t    errmsg;
};

static const struct _img_error img_error_arr[] = {
    {IMGERR_INVALID_IFOBJ,  "invalid img file object"},
    {IMGERR_INVALID_MEMBUF, "invalid mem buffer"},
    {IMGERR_INVALID_FILDES, "invalid file desc"},
    {IMGERR_INVALID_ICUR,   "invalid file cursor"},
    {IMGERR_NULL_PTR,       "nullptr"},
    {IMGERR_NULL_BUF,       "null buffer"},
    {IMGERR_WRITE_FAILED,   "failed to write to img file"},
    {IMGERR_FILE_FULL,      "img file full"},
    {IMGERR_CALC_CRC_FAILED, "failed to calculate crc32"},
    {IMGERR_INVALID_CRC,    "invalid crc32"},
    {IMGERR_INVALID_ENTRY_COUNT, "invalid entry count"}
};

static const size_t errc = sizeof(img_error_arr) / sizeof(img_error_arr[0]);

static inline strlit_t img_strerror(int img_errno) {
    for (size_t i = 0; i < errc; ++i) {
        if (img_error_arr[i].img_errno == img_errno)
            return img_error_arr[i].errmsg;
    }
    return "no error";
}

#define IMG_FILE_HEADER_SIZE    sizeof(struct _img_file_header_base)
#define IMG_TOC_ENTRY_SIZE      sizeof(struct _toc_entry)

struct _img_file_header_base {
    qword_t     ifh_magic;          // [0]
    word_t      ifh_version;        // [1]
    word_t      ifh_flags;          // [2]
    word_t      ifh_alignsize;      // [3]
    size32_t    ifh_headersize;     // [4]
    dword_t     ifh_entrycount;     // [5]
    size32_t    ifh_toc_entrysize;  // [6]
    uoff64_t    ifh_tocoff;         // [7]
    uoff64_t    ifh_dataoff;        // [8]
    size64_t    ifh_filesize;       // [9]
};
struct _img_file_header_payload {
    dword_t     ifh_header_crc32;   // [10]
    dword_t     ifh_toc_crc32;      // [11]
};
typedef struct _img_file_header {
    struct _img_file_header_base    ifh_base;       // [0]
    struct _img_file_header_payload ifh_payload;    // [1]
} __attribute__((aligned(ALIGN_SIZE))) IMG_FILE_HEADER;


typedef struct _toc_entry {
    dword_t     entry_id;       // [0]
    word_t      entry_type;     // [1]
    word_t      entry_flags;    // [2]
    size32_t    entry_size;     // [3]
    uoff64_t    entry_off;      // [4]
    dword_t     entry_crc32;    // [5]
} __attribute__((aligned(ALIGN_SIZE))) IMG_TOC_ENTRY;


typedef struct _img_file_obj {
    int                 if_desc;        // [0]
    void               *if_mem;         // [1]
    size64_t            if_mem_size;    // [2]
    uoff64_t            if_cur;         // [3]
    IMG_FILE_HEADER    *if_hdr_p;       // [4]
    IMG_TOC_ENTRY      *if_toc_p;       // [5]
    addr_t              if_data_addr;   // [6]
    dword_t             if_entry_cnt;   // [7]
    dword_t             if_entry_written; // [8]
    int                 if_errno;       // [9]
} IMG_FILE_OBJECT;


#define IMG_TOC_OFF sizeof(IMG_FILE_HEADER)
#define IMG_DATA_OFF(entrycnt) (sizeof(IMG_FILE_HEADER) + entrycnt * sizeof(IMG_TOC_ENTRY))

static inline IMG_FILE_HEADER *img_get_fileheader_ptr(addr_t base_addr) {
    return (IMG_FILE_HEADER *)base_addr;
}

static inline IMG_TOC_ENTRY *img_get_toc_ptr(addr_t base_addr) {
    return (IMG_TOC_ENTRY *)(base_addr + IMG_TOC_OFF);
}

static inline addr_t img_get_data_addr(addr_t base_addr, dword_t entrycnt) {
    return base_addr + IMG_DATA_OFF(entrycnt);
}


static inline void IMG_fill_fileheader(
    IMG_FILE_HEADER    *hdr_p,
    dword_t             entrycnt,
    word_t              flags
) {
    if (!hdr_p) return;
    hdr_p->ifh_base.ifh_magic       = *(qword_t*)IMG_FILE_MAGIC;
    hdr_p->ifh_base.ifh_version     = VERSION;
    hdr_p->ifh_base.ifh_flags       = flags;
    hdr_p->ifh_base.ifh_alignsize   = ALIGN_SIZE;
    hdr_p->ifh_base.ifh_headersize  = sizeof(IMG_FILE_HEADER);
    hdr_p->ifh_base.ifh_entrycount  = entrycnt;
    hdr_p->ifh_base.ifh_toc_entrysize = sizeof(IMG_TOC_ENTRY);
    hdr_p->ifh_base.ifh_tocoff      = IMG_TOC_OFF;
    hdr_p->ifh_base.ifh_dataoff     = IMG_DATA_OFF(entrycnt);
    hdr_p->ifh_base.ifh_filesize    = 0;
}

static inline void _IMG_FILE_update_fileheader(
    IMG_FILE_HEADER    *hdr_p,
    uoff64_t            eof_cur,
    ulong_t             hdr_crc,
    ulong_t             toc_crc
) {
    if (!hdr_p) return;
    hdr_p->ifh_base.ifh_filesize        = eof_cur;
    hdr_p->ifh_payload.ifh_header_crc32 = hdr_crc;
    hdr_p->ifh_payload.ifh_toc_crc32    = toc_crc;
}


static inline int IMG_FILE_OBJECT_init(
    IMG_FILE_OBJECT    *_this,
    int                 fd,
    void               *mem,
    size64_t            memsize,
    dword_t             entrycnt,
    word_t              flags
) {
    if (!_this) return IMGERR_INVALID_IFOBJ;
    if (!mem || memsize < IMG_DATA_OFF(entrycnt))
        return IMGERR_INVALID_MEMBUF;
    if (fd < 0) return IMGERR_INVALID_FILDES;

    _this->if_desc      = fd;
    _this->if_mem       = mem;
    _this->if_mem_size  = memsize;
    _this->if_cur       = IMG_DATA_OFF(entrycnt);
    _this->if_hdr_p     = img_get_fileheader_ptr((addr_t)mem);
    _this->if_toc_p     = img_get_toc_ptr((addr_t)mem);
    _this->if_data_addr = img_get_data_addr((addr_t)mem, entrycnt);
    _this->if_entry_cnt = entrycnt;
    _this->if_entry_written = 0;
    _this->if_errno     = 0;

    IMG_fill_fileheader(_this->if_hdr_p, entrycnt, flags);

    lseek(fd, IMG_DATA_OFF(entrycnt), SEEK_SET);


    return entrycnt;
}

static inline int _IMG_FILE_OBJECT_validate(IMG_FILE_OBJECT *_this) {
    // validate object internal fields
    if (!_this)
        return IMGERR_INVALID_IFOBJ;
    if (!_this->if_hdr_p || !_this->if_toc_p || !_this->if_mem)
        return IMGERR_NULL_PTR;
    if (_this->if_desc < 0)
        return IMGERR_INVALID_FILDES;
    if (_this->if_entry_written > _this->if_entry_cnt) 
        return IMGERR_INVALID_ENTRY_COUNT;
    return 0;
}

static inline int _IMG_FILE_OBJECT_write_validate(IMG_FILE_OBJECT *_this) {
    // validation for write
    int ret = _IMG_FILE_OBJECT_validate(_this);
    if (ret < 0) return ret;

    if (_this->if_cur < IMG_DATA_OFF(_this->if_entry_cnt))
        return IMGERR_INVALID_ICUR;
    if (_this->if_entry_written >= _this->if_entry_cnt) 
        return IMGERR_FILE_FULL;
    return 0;
}

static inline ssize_t _IMG_FILE_write_entry_data_seqio(
    int             fd,
    uoff64_t       *_file_cur_p,
    size32_t        buf_len,
    void           *buf
) {
    // this function would advance the file cursor
    // and its conceptually similar to POSIX write;
    // this also aligns the offset of the next payload (aka the cursor)
    // to ALIGN_SIZE
    if (fd < 0) return IMGERR_INVALID_FILDES;   // invalid file desc
    if (!buf) return IMGERR_NULL_BUF;           // NULL buffer

    //print_dbg_msg("cur=%llu\n", *_file_cur_p);
    ssize_t written = write_full(fd, buf, buf_len);         // this completes write by looping over
    if (written != buf_len) return IMGERR_WRITE_FAILED;

    *_file_cur_p += written;                                // advance cur
    *_file_cur_p = ALIGN_OFFSET(*_file_cur_p, ALIGN_SIZE);  // align cur to ALIGN_SIZE
    lseek64(fd, *_file_cur_p, SEEK_SET);                    // update fd internal cur

    return written;
}

static inline void _IMG_FILE_toc_write_entry(
    IMG_TOC_ENTRY  *toc_p,
    dword_t         entry_idx,
    word_t          entry_type,
    word_t          entry_flags,
    size32_t        entry_size,
    uoff64_t        entry_off,
    ulong_t         entry_crc
) {
    // it fills TOC
    if (!toc_p) return;
    toc_p[entry_idx].entry_id     = entry_idx;
    toc_p[entry_idx].entry_type   = entry_type;
    toc_p[entry_idx].entry_flags  = entry_flags;
    toc_p[entry_idx].entry_size   = entry_size;
    toc_p[entry_idx].entry_off    = entry_off;
    toc_p[entry_idx].entry_crc32  = entry_crc;
}

static inline int IMG_FILE_write_entry_seqio(
    IMG_FILE_OBJECT    *_this,
    word_t              entry_type,
    word_t              entry_flags,
    size32_t            buf_len,
    void               *buf
) {
    // write entry for equential io
    // validate this
    int ret = _IMG_FILE_OBJECT_write_validate(_this);
    if (ret < 0) return ret;

    // entry off    : file cur;
    // entry index  : entry written count
    uoff64_t entry_off = _this->if_cur;
    dword_t  entry_idx = _this->if_entry_written;

    // write data payload
    ssize_t written = _IMG_FILE_write_entry_data_seqio(
        _this->if_desc, &_this->if_cur, buf_len, buf
    );
    if (written < 0) return IMGERR_WRITE_FAILED;

    // write toc entry
    ulong_t entry_crc = compute_mem_crc32(buf, buf_len);
    if (!entry_crc) return IMGERR_CALC_CRC_FAILED;

    _IMG_FILE_toc_write_entry(
        _this->if_toc_p,
        entry_idx,
        entry_type,
        entry_flags,
        buf_len,
        entry_off,
        entry_crc
    );
    ++_this->if_entry_written; // inc written count

    return (int)entry_idx;
}

static inline int IMG_FILE_flush_seqio(IMG_FILE_OBJECT *_this) {
    int ret = _IMG_FILE_OBJECT_validate(_this);
    if (ret < 0) return ret;
    dword_t hdr_crc = compute_mem_crc32(
        _this->if_hdr_p,
        sizeof(struct _img_file_header_base)
    );
    dword_t toc_crc = compute_mem_crc32(
        _this->if_toc_p,
        _this->if_entry_cnt * sizeof(IMG_TOC_ENTRY)
    );
    if (!hdr_crc || !toc_crc) return IMGERR_CALC_CRC_FAILED;

    _IMG_FILE_update_fileheader(
        _this->if_hdr_p,
        _this->if_cur,
        hdr_crc,
        toc_crc
    );

    // seek to beginning of mem
    lseek(_this->if_desc, 0, SEEK_SET);
    ssize_t written = write_full(
        _this->if_desc,
        _this->if_mem,
        _this->if_mem_size
    );
    if (written < 0) return IMGERR_WRITE_FAILED;

    return 0;
}


static inline void IMG_FILE_OBJECT_deinit(IMG_FILE_OBJECT *_this) {
    if (!_this) return;
    *_this = (IMG_FILE_OBJECT) {0}; // memory and res are owned by caller
}


#endif /*#ifndef BINARY_IMAGE_H*/