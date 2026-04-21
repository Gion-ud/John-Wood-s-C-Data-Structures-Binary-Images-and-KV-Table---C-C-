#pragma once

#include <stddef.h>
static void local_memset(void *buf_p, unsigned char ch, size_t len) {
    if (!buf_p) return;
    unsigned char *ptr = (unsigned char*)buf_p;
    unsigned char *end_ptr = (unsigned char*)buf_p + len;
    while (ptr != end_ptr) *ptr++ = ch;
}
static void local_memcpy(void *dest_p, const void *src_p, size_t len) {
    if (!dest_p || !src_p) return;
    size_t i = 0;
    unsigned char *dest_p1 = (unsigned char*)dest_p;
    unsigned char *src_p1 = (unsigned char*)src_p;
    while (i < len) {
        dest_p1[i] = src_p1[i];
        ++i;
    }
}
static int local_memcmp(const void *buf1_p, const void *buf2_p, size_t len) {
    if (!buf1_p || !buf2_p) return 0;
    size_t i = 0;
    unsigned char *ptr1 = (unsigned char*)buf1_p;
    unsigned char *ptr2 = (unsigned char*)buf2_p;
    while (i < len) {
        if (ptr1[i] != ptr2[i]) return (int)ptr1[i] - (int)ptr2[i];
        ++i;
    }
    return 0;
}
static int local_strcmp(const char *cstr1_p, const char *cstr2_p) {
    if (!cstr1_p || !cstr2_p) return 0;
    size_t i = 0;
    while (cstr1_p[i] && cstr2_p[i]) {
        if (cstr1_p[i] != cstr2_p[i]) break;
        ++i;
    }
    return (int)cstr1_p[i] - (int)cstr2_p[i];
}
static size_t local_strlen(const char *cstr_p) {
    if (!cstr_p) return 0;
    size_t len = 0;
    const char *p = cstr_p;
    while (*p) {
        ++len; ++p;
    }
    return len;
}