#include <posix_io.h>
#include <unistd.h>
#include <global_intdef.h>
#include <global_ptrdef.h>
#include <dbg_print.h>
#include <sys/types.h>
#include <global.h>
#include <stdbool.h>

#include "page.h"

static inline bool _PAGE_FILE_OBJECT_VALIDATE(PAGE_FILE_OBJECT *_this, int *_page_errno_p) {
    if (is_null(_this)) {
        *_page_errno_p = PAGE_INVALID_PGFOBJ;
        goto _PFO_VALIDATE_false;
    }
    if (is_null(_this->pfo_file_mem)) {
        *_page_errno_p = PAGE_INVALID_MEMBUF;
        goto _PFO_VALIDATE_false;
    }
    if (_this->pfo_file_desc < 0) {
        *_page_errno_p = PAGE_INVALID_FILDES;
        goto _PFO_VALIDATE_false;
    }
    return true;
_PFO_VALIDATE_false:
    return false;
}

static inline bool _PAGE_FILE_validate_page_idx(
    size32_t            page_cnt,
    size64_t            page_file_size,
    ulong_t             page_idx,
    int                *_page_errno_p
) {
    if (
        page_idx >= page_cnt ||
        page_idx * PAGE_SIZE >= page_file_size
    ) {
        *_page_errno_p = PAGE_INVALID_PGNO;
        return false;
    }
    return true;
}

static inline bool _page_validate_page_cur(uoff32_t page_cur, int *_page_errno_p) {
    if (page_cur >= PAGE_SIZE) {
        *_page_errno_p = PAGE_INVALID_PGCUR;
        return false;
    }
    return true;
}

int PAGE_FILE_OBJECT_init(
    PAGE_FILE_OBJECT   *_this,
    int                 page_file_desc,
    void               *page_file_mem,
    size64_t            page_file_size,
    ulong_t             page_count
) {
    if (is_null(_this)) {
        return PAGE_INVALID_PGFOBJ;
    }
    int err = 0;
    if ((PAGE_SIZE & (PAGE_SIZE - 1)) != 0) {
        err = PAGE_SIZE_NOT_POWER_OF_2;
        goto PFO_init_cleanup;
    }
    if (page_file_desc < 0) {
        err = PAGE_INVALID_FILDES;
        goto PFO_init_cleanup;
    }
    if (is_null(page_file_mem)) {
        err = PAGE_INVALID_MEMBUF;
        goto PFO_init_cleanup;
    }

    _this->pfo_file_desc    = page_file_desc;
    _this->pfo_file_mem     = page_file_mem;
    _this->pfo_file_size    = page_file_size;
    _this->pfo_page_cnt     = page_count;
    _this->pfo_file_cur     = sizeof(PAGE_FILE_HEADER);
    _this->pfo_page_idx     = 0;
    _this->pfo_page_cur     = sizeof(PAGE_FILE_HEADER);
    _this->pfo_errno        = 0;


    return page_count;
PFO_init_cleanup:
    return err;
}

int PAGE_FILE_goto_page(
    PAGE_FILE_OBJECT   *_this,
    ulong_t             page_idx
) {
    if (!_PAGE_FILE_OBJECT_VALIDATE(_this, &_this->pfo_errno) ||
        !_PAGE_FILE_validate_page_idx(
            _this->pfo_page_cnt, _this->pfo_file_size, page_idx, &_this->pfo_errno
        )
    )
        return _this->pfo_errno;

    _this->pfo_page_idx = page_idx;
    if (page_idx != 0) {
        _this->pfo_page_cur = _this->pfo_page_cur;
    } else {
        _this->pfo_page_cur = sizeof(PAGE_FILE_HEADER);
    }

    return page_idx;
}

int PAGE_FILE_page_read_buffer_raw(
    PAGE_FILE_OBJECT   *_this,
    ulong_t             page_idx,
    uoff32_t            page_cur,
    size_t              buf_len,
    byte_t             *out_buf_p
) {
    if (!_PAGE_FILE_OBJECT_VALIDATE(_this, &_this->pfo_errno) ||
        !_PAGE_FILE_validate_page_idx(
            _this->pfo_page_cnt, _this->pfo_file_size, page_idx, &_this->pfo_errno
        ) ||
        !_page_validate_page_cur(page_cur, &_this->pfo_errno) ||
        !_page_validate_page_cur(page_cur + buf_len, &_this->pfo_errno)
    )
        return _this->pfo_errno;
    if (!out_buf_p) {
        _this->pfo_errno = PAGE_NULL_BUFFER;
        return _this->pfo_errno;
    }
    addr_t base_addr = (addr_t)_this->pfo_file_mem;
    byte_t *src = (byte_t*)base_addr + PAGE_SIZE * page_idx + page_cur;
    memcpy(out_buf_p, src, buf_len);
    return page_idx;
}


int PAGE_FILE_page_write_buffer_raw(
    PAGE_FILE_OBJECT   *_this,
    ulong_t             page_idx,
    uoff32_t            page_cur,
    byte_t              buf[],
    size_t              buf_len
) {
    if (!_PAGE_FILE_OBJECT_VALIDATE(_this, &_this->pfo_errno) ||
        !_PAGE_FILE_validate_page_idx(
            _this->pfo_page_cnt, _this->pfo_file_size, page_idx, &_this->pfo_errno
        ) ||
        !_page_validate_page_cur(page_cur, &_this->pfo_errno) ||
        !_page_validate_page_cur(page_cur + buf_len, &_this->pfo_errno)
    )
        return _this->pfo_errno;
    if (!buf) {
        _this->pfo_errno = PAGE_NULL_BUFFER;
        return _this->pfo_errno;
    }
    addr_t base_addr = (addr_t)_this->pfo_file_mem;
    byte_t *dest = (byte_t*)base_addr + PAGE_SIZE * page_idx + page_cur;
    memcpy(dest, buf, buf_len);
    return page_idx;
}

void PAGE_FILE_OBJECT_deinit(
    PAGE_FILE_OBJECT   *_this
) {
    if (is_null(_this)) return;
    *_this = (PAGE_FILE_OBJECT) {0};    // internal buffers are owned by the user
}


