#ifndef HASH_FUNC_H
#define HASH_FUNC_H

#include "hash_def.h"

static const qword_t FNV_OFFSET_BASIS_QWORD = 0xcbf29ce484222325ULL;
static const qword_t FNV_PRIME_QWORD        = 0x100000001b3ULL;
static const dword_t FNV_OFFSET_BASIS_DWORD = 0x811C9DC5;
static const dword_t FNV_PRIME_DWORD        = 0x01000193;


static inline hash64_t fnv_1a_hash64(const byte_t* key, size_t len) {
    qword_t h = FNV_OFFSET_BASIS_QWORD;
    size_t i = 0;
    while (i < len) {
        h ^= key[i++];
        h *= FNV_PRIME_QWORD;
    }
    return h;
}

static inline hash32_t fnv_1a_hash32(const byte_t* key, size_t len) {
    dword_t h = FNV_OFFSET_BASIS_DWORD;
    size_t i = 0;
    while (i < len) {
        h ^= key[i++];
        h *= FNV_PRIME_DWORD;
    }
    return h;
}

#endif