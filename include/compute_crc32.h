#pragma once

#include <zlib.h>
#include "libc_chked.h"
#include <posix_io.h>

static inline ulong_t compute_file_section_crc32(
    FILE       *this_file_p,
    uoff32_t    src_off,
    uoff32_t    dest_off
) {
    if (is_null(this_file_p)) return 0;
    ulong_t f_crc32 = (ulong_t)crc32(0L, Z_NULL, 0);
    byte_t buffer[BUFFER_SIZE] = {0};
    size_t n = 0;
    fseek(this_file_p, src_off, SEEK_SET);
    for (uoff32_t i = 0; i < dest_off / BUFFER_SIZE; ++i) {
        n = fread(buffer, 1, BUFFER_SIZE, this_file_p);
        if (!n) break;
        f_crc32 = crc32(f_crc32, (byte_t*)buffer, n);
    }
    size_t rem = dest_off % BUFFER_SIZE;
    if (rem) {
        n = fread(buffer, 1, rem, this_file_p);
        if (n) {
            f_crc32 = crc32(f_crc32, (byte_t*)buffer, n);
        }
    }
    return f_crc32;
}


static inline ulong_t compute_file_crc32_fildes(
    int         fd,
    uoff64_t    off,
    size32_t    len
) {
    if (fd < 0) return 0;
    ulong_t f_crc32 = (ulong_t)crc32(0L, Z_NULL, 0);
    byte_t buffer[BUFFER_SIZE] = {0};
    ssize_t n = 0;
    lseek(fd, off, SEEK_SET);
    for (uoff32_t i = 0; i < len / BUFFER_SIZE; ++i) {
        n = read_full(fd, (byte_t*)buffer, BUFFER_SIZE);
        if (!n) break;
        if (n < 0) return 0;
        f_crc32 = crc32(f_crc32, (byte_t*)buffer, n);
    }
    size_t rem = len % BUFFER_SIZE;
    if (rem) {
        n = read_full(fd, (byte_t*)buffer, rem);
        if (n > 0) {
            f_crc32 = crc32(f_crc32, (byte_t*)buffer, n);
        }
        else if (n < 0) return 0;
    }
    return f_crc32;
}
/*

*/
static inline ulong_t compute_mem_crc32(void *buf, ulong_t buf_len) {
    if (is_null(buf)) return 0;
    ulong_t ulcrc32 = (ulong_t)crc32(0L, Z_NULL, 0);
    ulcrc32 = crc32(ulcrc32, buf, buf_len);
    return ulcrc32;
}