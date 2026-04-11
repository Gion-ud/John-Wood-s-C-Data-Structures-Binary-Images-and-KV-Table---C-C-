#ifndef __BUFFER_H__
#define __BUFFER_H__

extern "C" {
#include <tlv/tlv.h>
#include <string.h>
#include <stdlib.h>
}

#ifdef __cplusplus
extern "C" {
#endif

static inline const LPBuffer *cstr_to_lpbuf_rdonly(LPBuffer *lpbuf_p, const char *sz_p) {
    if (!lpbuf_p || !sz_p) return NULL;
    *lpbuf_p = (LPBuffer) {
        .len = strlen(sz_p),
        .data = (void*)sz_p
    };
    return lpbuf_p;
}

static LPBuffer *create_LPBuffer(const char *src) {
    size_t len = strlen(src);
    LPBuffer *lpbuf_p = (LPBuffer*)malloc(sizeof(LPBuffer));
    if (!lpbuf_p) {
        return NULL;
    }
    lpbuf_p->len = len;
    lpbuf_p->data = malloc(len + 1);
    if (!lpbuf_p->data) {
        free(lpbuf_p);
        return NULL;
    }
    memcpy(lpbuf_p->data, src, len);
    ((char*)lpbuf_p->data)[len] = 0;
    return lpbuf_p;
}

static void destroy_LPBuffer(LPBuffer *lpbuf_p) {
    if (!lpbuf_p) return;
    if (lpbuf_p->data) free(lpbuf_p->data);
    free(lpbuf_p);
}

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

namespace dctr {

}


#endif

#endif /* __BUFFER_H__ */