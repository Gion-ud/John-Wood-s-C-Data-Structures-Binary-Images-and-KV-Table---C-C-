#ifndef __CSTR_H__
#define __CSTR_H__

#include <stddef.h>
#include <string.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif
static inline void line_rstrip(char *line_p) {
    if (!line_p) return;
    size_t line_len = strlen(line_p);
    if (!line_len) return;
    char *line_end_p = line_p + line_len - 1;
    while (line_end_p >= line_p && isspace((unsigned char)*line_end_p)) {
        *line_end_p-- = 0;
    }
}
static inline char *line_lstrip(char *line_p) {
    if (!line_p) return NULL;
    char *line_begin_p = line_p;
    while (*line_begin_p && isspace((unsigned char)*line_begin_p)) {
        ++line_begin_p;
    }
    return line_begin_p;
}

static inline char *line_strip(char *line_p) {
    if (!line_p) return NULL;
    line_rstrip(line_p);
    line_p = line_lstrip(line_p);
    return line_p;
}
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include <stdexcept>
#include <new>
#include <iostream>

class cstr {
    char *sz_p;
public:
    cstr() noexcept : sz_p(nullptr) {}
    cstr(cstr &&other) noexcept : sz_p(other.sz_p) {
        other.sz_p = nullptr;
    }
    explicit cstr(const char *sz_p) noexcept : sz_p(nullptr) {
        if (sz_p) this->copy_from_sz(sz_p);
    }
    char *create(size_t len) {
        if (!len) return nullptr;
        this->destroy();
        this->sz_p = (char*)operator new(len + 1, std::nothrow);
        if (!this->sz_p) {
            std::cerr << "operator new failed;";
            return nullptr;
        }
        this->sz_p[len] = 0;
        return this->sz_p;
    }
    void strip() {
        this->move_in(cstr(line_strip(this->sz_p)));
    }
    char *copy_from_sz(const char *src_p) noexcept {
        if (!src_p) return nullptr;
        size_t len = strlen(src_p);
        if (!this->create(len)) return nullptr;
        memcpy(this->sz_p, src_p, len);
        return this->sz_p;
    }
    void move_in(cstr &&in) noexcept {
        if (this == &in) return;
        this->destroy();
        this->sz_p = in.sz_p;
        in.sz_p = nullptr;
    }
    cstr move_out() noexcept {
        cstr out;
        out.sz_p = this->sz_p;
        this->sz_p = nullptr;
        return out;
    }
    void copy_from(const cstr &in) noexcept {
        if (this == &in || !in.sz_p) return;
        this->copy_from_sz(in.sz_p);
    }
    cstr copy() const noexcept {
        cstr out;
        out.copy_from(*this);
        return out;
    }
    cstr(const cstr &cstr_obj) = delete;
    cstr &operator=(const cstr &cstr_obj) = delete;
    const char *data() const noexcept {
        return this->sz_p;
    }
    void destroy() noexcept {
        if (sz_p) operator delete(sz_p);
        this->sz_p = nullptr;
    }
    ~cstr() noexcept {
        this->destroy();
    }
};
#endif

#ifdef __STDC__
#include <stdlib.h>
typedef char *cstr_t;
static inline char *create_cstr(size_t len) {
    char *str_p = (char*)malloc(len + 1);
    if (!str_p) return NULL;
    str_p[len] = 0;
    return str_p;
}
static inline char *cstr_copy_from_sz(const char *src_p) {
    size_t len = strlen(src_p);
    char *sz_p = create_cstr(len);
    if (!sz_p) return NULL;
    memcpy(sz_p, src_p, len);
    return sz_p;
}

static inline void destroy_cstr(char *str_p) {
    if (str_p) free(str_p);
}
#endif



#endif