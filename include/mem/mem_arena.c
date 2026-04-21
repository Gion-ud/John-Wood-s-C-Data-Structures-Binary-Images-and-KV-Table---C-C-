#include "mem_arena.h"
#include <assert.h>

ptrdiff_t mem_arena_init(
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

    return (ptrdiff_t)mem_size;
}

/*
ptrdiff_t mem_arena_addr_to_memcur(MemArena *_this, void *addr) {
    if (!_this || !_this->mem_buf || !addr) return -1;
    return (ptrdiff_t)addr - (ptrdiff_t)_this->mem_buf;
}

*/

ptrdiff_t mem_arena_get_last_alloc_memcur(MemArena *_this, size_t size) {
    if (!_this || !_this->mem_buf || !size || _this->mem_cur < size) return -1;
    return _this->mem_cur - size;
}

static inline ptrdiff_t _mem_arena_get_next_alloc_memcur(MemArena *_this, size_t size) {
    size_t mem_cur = ALIGN_OFFSET(_this->mem_cur, _this->mem_align);
    if (mem_cur + size > _this->mem_size) return -1;
    return mem_cur;
}

ptrdiff_t mem_arena_get_next_alloc_memcur(MemArena *_this, size_t size) {
    if (!_this || !_this->mem_buf || !size) return -1;
    return _mem_arena_get_next_alloc_memcur(_this, size);
}

static inline void *_mem_arena_alloc(MemArena *_this, size_t size) {
    assert(_this);
    assert(_this->mem_buf);
    assert(size);
    size_t mem_cur = ALIGN_OFFSET(_this->mem_cur, _this->mem_align);
    assert(_mem_arena_get_next_alloc_memcur(_this, size) >= 0);
    if (
        _mem_arena_get_next_alloc_memcur(_this, size) < 0
    ) return NULL;

    _this->mem_cur = mem_cur + size;
    ++_this->mem_alloc_cnt;

    return (unsigned char*)_this->mem_buf + mem_cur;
}

void *mem_arena_alloc(MemArena *_this, size_t size) {
    if (!_this || !_this->mem_buf || !size) return NULL;
    return _mem_arena_alloc(_this, size);
}

void *mem_arena_alloc_arr(MemArena *_this, size_t count, size_t elem_size) {
    if (!_this || !_this->mem_buf || !count || !elem_size) return NULL;
    return _mem_arena_alloc(_this, count * elem_size);
}



size_t mem_arena_getcur(MemArena *_this) {
    if (!_this || !_this->mem_buf) return 0;
    return _this->mem_cur;
}

void mem_arena_setcur(MemArena *_this, size_t prev_pos) {
    if (
        !_this ||
        !_this->mem_buf ||
        prev_pos > _this->mem_cur
    ) return;
    _this->mem_cur = prev_pos;
}

void mem_arena_reset(MemArena *_this) {
    if (!_this ||!_this->mem_buf) return;
    _this->mem_cur = 0;
    _this->mem_alloc_cnt = 0;
    return;
}