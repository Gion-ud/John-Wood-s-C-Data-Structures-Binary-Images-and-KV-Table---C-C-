#include "page.h"
#include <posix_io.h>
#include <global_intdef.h>
#include <global_ptrdef.h>
#include <dbg_print.h>
#include <global.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>


static const char *filename         = "pagedfile.bin";
static int page_errno               = 0;
static const size32_t page_count    = 8;
static const size64_t memsize       = page_count * PAGE_SIZE;

int main() {
    int fd = open(
        filename, O_RDWR | O_CREAT
#ifdef _WIN32
        | O_BINARY
#endif
        ,
        0644
    );
    if (fd < 0) {
        perror("open");
        goto main_cleanup;
    }

    if (ftruncate(fd, memsize) < 0) {
        perror("ftruncate");
        goto main_cleanup;
    }

    void *mem = mmap(
        NULL,
        memsize,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        0
    );
    if (mem == MAP_FAILED) {
        perror("mmap");
        goto main_cleanup;
    }
    addr_t base_addr = (addr_t)mem;
    print_dbg_msg("base_addr: 0x%.16llx\n", base_addr);

    PAGE_FILE_OBJECT p_obj = {0};
    page_errno = PAGE_FILE_OBJECT_init(&p_obj, fd, mem, memsize, page_count);
    if (page_errno < 0) {
        printerrf("PAGE_FILE_OBJECT_init failed: %s\n", page_strerror(page_errno));
        goto main_cleanup;
    }

    PAGE_FILE_HEADER *pagefileheader_p = page_get_fileheader_ptr(base_addr);
    //PAGE_HEADER *pageheader_p = NULL;
    //PAGE_FOOTER *pagefooter_p = NULL;

    for (size32_t i = 0; i < page_count; ++i) {
        page_fill_pageheader(base_addr, FLAG_NONE, i);
        page_fill_pagefooter(base_addr, i);
    }

    page_fill_fileheader(
        pagefileheader_p,
        FLAG_NONE,
        page_count,
        DEFAULT_INDEX_PAGE_IDX
    );
    PAGE_FILE_OBJECT_deinit(&p_obj);
    msync(mem, memsize, MS_ASYNC);
    munmap(mem, memsize);
    close(fd);

    return 0;
main_cleanup:
    int err = errno;
    if (mem && mem != MAP_FAILED) munmap(mem, memsize);
    if (fd > STDERR_FILENO) close(fd);

    return page_errno;
}