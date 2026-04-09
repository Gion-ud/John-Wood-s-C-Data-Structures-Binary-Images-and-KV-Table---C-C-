#include "mem_arena.h"


long long mem_arena_init(
    MemArena   *_this,
    void       *mem_buf,
    size_t      mem_size,
    size_t      mem_align
) {
    if (!_this ||!mem_buf) return -1;

    _this->mem_size     = mem_size;
    _this->mem_align    = mem_align;
    _this->mem_cur      = 0;
    _this->mem_buf      = mem_buf;
    _this->mem_alloc_cnt = 0;

    return (long long)mem_size;
}

void *mem_arena_alloc(MemArena *_this, size_t size) {
    if (!_this || !_this->mem_buf || !size) return NULL;
    size_t mem_cur = ALIGN_OFFSET(_this->mem_cur, _this->mem_align);
    if (mem_cur + size > _this->mem_size) return NULL;

    _this->mem_cur = mem_cur + size;
    ++_this->mem_alloc_cnt;

    return (unsigned char*)_this->mem_buf + mem_cur;
}

void *mem_arena_alloc_memcur(
    MemArena   *_this,
    size_t      size,
    size_t     *out_mem_cur_p
) {
    if (!_this || !_this->mem_buf || !size || !out_mem_cur_p) return NULL;
    size_t mem_cur_before = ALIGN_OFFSET(_this->mem_cur, _this->mem_align);
    size_t mem_cur_after = mem_cur_before + size;
    if (mem_cur_after > _this->mem_size)
        return NULL;

    _this->mem_cur = mem_cur_after;
    *out_mem_cur_p = mem_cur_before;
    ++_this->mem_alloc_cnt;

    return (unsigned char*)_this->mem_buf + mem_cur_before;
}