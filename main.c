#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <kvtable/kvtable.h>
#include <c_std_vector/vector.h>
#include <tlv/tlv.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <kvtable/kv_ser.h>

static inline LPBuffer *cstr_to_lpbuf(LPBuffer *lpbuf_p, const char *strlit_p) {
    if (!lpbuf_p || !strlit_p) return NULL;
    *lpbuf_p = (LPBuffer) {
        .len = strlen(strlit_p),
        .data = (void*)strlit_p
    };
    return lpbuf_p;
}

static char line_buf[256] = {0};

#ifndef O_BINARY
#define O_BINARY 0x8000
#endif

LPBuffer *create_LPBuffer(const char *src) {
    size_t len = strlen(src);
    LPBuffer *lpbuf_p = (LPBuffer*)malloc(sizeof(LPBuffer));
    if (!lpbuf_p) {
        puts("malloc LPBuffer failed");
        return NULL;
    }
    lpbuf_p->len = len;
    lpbuf_p->data = malloc(len + 1);
    if (!lpbuf_p->data) {
        puts("malloc lpbuf_p->data failed\n");
        free(lpbuf_p);
        return NULL;
    }
    memcpy(lpbuf_p->data, src, len);
    ((char*)lpbuf_p->data)[len] = 0;
    return lpbuf_p;
}

void destroy_LPBuffer(LPBuffer *lpbuf_p) {
    if (!lpbuf_p) return;
    if (lpbuf_p->data) free(lpbuf_p->data);
    free(lpbuf_p);
}

#include "cstr.h"

int main() {
    KVTableHandle kvt_h = new_KVTable(32, MEM_SIZE);
    assert(kvt_h);

    Vector key_vec = {0};
    Vector_init_assign_literal(char*, &key_vec, {
        "Google", "ChatGPT", "YouTube", "W3Schools",
        "Gemini", "CC", "CXX", "OS", "kernel", "ExecFormat",
        "ObjFormat", "Arch", "DefaultBrowser", "Country",
        "Company", "text_editor", "CodeEditor", "c version",
        "c++ version", "Python version", "allocator", "c runtime",
        "c++ runtime", "DefaultToolchain"}
    );
    puts("keys:");
    for (char **it = Vector_begin(char*, &key_vec); it != Vector_end(char*, &key_vec); ++it) {
        printf("%s\n", *it);
    }
    puts("end\n\n");

    Vector *val_vec_p = new_Vector(char*);
    assert(val_vec_p);
    puts("Enter corresponding vals");
    char *line_p = line_buf;
    for (size_t i = 0; i < Vector_size(&key_vec); ++i) {
        printf("%.4zu\tkey:%s -> val:\n\t", i, *Vector_get(char*, &key_vec, i));
        if (!fgets(line_p, sizeof(line_buf), stdin)) break;
        if (strlen(line_p) == sizeof(line_buf) - 1) {
            puts("line too long");
            break;
        }
        line_p = line_strip(line_p);
        //if (!strlen(line_p)) break;
        char *val_sz_p = (strlen(line_p)) ? cstr_copy_from_sz(line_p) : NULL;
        //if (!val_sz_p) continue;
        if (!Vector_push(char*, val_vec_p, &val_sz_p)) {
            puts("Vector_push failed");
            destroy_cstr(val_sz_p);
            continue;
        }
        if (!strlen(line_p)) break;
    }
    for (size_t i = 0; i < Vector_size(&key_vec); ++i) {
        char *key_str_p = *Vector_get(char*, &key_vec, i);
        char *val_str_p = *Vector_get(char*, val_vec_p, i);
        if (!key_str_p || !*key_str_p || !val_str_p || !*val_str_p) break;
        printf(
            "%.4zu\tkey:%s -> val:%s\n", i,
            key_str_p,
            val_str_p
        );
    }
    puts("done getting vals");
    for (size_t i = 0; i < Vector_size(&key_vec); ++i) {
        char *key_data_p = *Vector_get(char*, &key_vec, i);
        char *val_data_p = *Vector_get(char*, val_vec_p, i);
        LPBuffer key, val;
        KVTableHandle kvt_tmp_h = KVTable_insert(
            kvt_h,
            cstr_to_lpbuf(&key, key_data_p),
            cstr_to_lpbuf(&val, val_data_p)
        );
        if (!kvt_tmp_h) break;
        kvt_h = kvt_tmp_h;
    }
    puts("done");
    int fd = open("data.bin", O_RDWR | O_CREAT | O_BINARY, 0644);
    assert(fd > 2);
    assert(!KVTable_serialise(kvt_h, fd));
    close(fd);
    while (1) {
        LPBuffer key, val;
        puts("Enter key to retrive val:");
        if (!fgets(line_p, sizeof(line_buf), stdin)) break;
        if (strlen(line_p) == sizeof(line_buf) - 1) {
            puts("line too long");
            continue;
        }
        line_p = line_strip(line_p);
        if (!strlen(line_p)) break;
        if (
            KVTable_lookup(
                kvt_h,
                cstr_to_lpbuf(&key, line_p),
                &val
            ) < 0
        ) {
            puts("key not found");
            continue;
        }
        printf(
            "kvt_h['%s']='%.*s'\n",
            line_p, (int)val.len, (char*)val.data
        );
    }

    for (char **it = Vector_begin(char*, val_vec_p); it != Vector_end(char*, val_vec_p); ++it) {
        if (*it) destroy_cstr(*it);
    }
    delete_Vector(val_vec_p);
    Vector_deinit(&key_vec);
    delete_KVTable(kvt_h);
    return 0;
}