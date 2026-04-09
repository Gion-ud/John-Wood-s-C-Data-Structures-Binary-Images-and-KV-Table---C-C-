#include "bipf/imgpch.h"

static const char *filename = "assets.img";

static const int idx = 0;
int main() {
    int fd = open(
        filename, O_RDONLY
#ifdef _WIN32
        | O_BINARY
#endif
    );
    if (fd < 0) {
        perror("open");
        goto main_cleanup;
    }

    struct stat st;
    fstat(fd, &st);
    size_t filesize = st.st_size;

    void *mem = mmap(
        NULL,
        filesize,
        PROT_READ,
        MAP_SHARED | MAP_FILE,
        fd,
        0
    );
    if (mem == MAP_FAILED) {
        perror("mmap");
        goto main_cleanup;
    }
    print_dbg_msg("base_addr: 0x%.16llx\n", (addr_t)mem);

    IMG_FILE_HEADER *img_hdr_p = img_get_fileheader_ptr((addr_t)mem);
    IMG_TOC_ENTRY *img_toc_p = img_get_toc_ptr((addr_t)mem);
    ptr_t data = (byte_t*)mem + img_toc_p[idx].entry_off;

    dword_t entrycnt = img_hdr_p->ifh_base.ifh_entrycount;
    if (
        idx >= entrycnt ||
        img_toc_p[idx].entry_off + img_toc_p[idx].entry_size > filesize
    )
        goto main_cleanup;
    
    printerrf("0x%.16llx\n", img_toc_p[idx].entry_off);

    int fd_entry = open("data.jpg",
        O_RDWR | O_CREAT | O_TRUNC
#ifdef _WIN32
        | O_BINARY
#endif
        ,
        0644
    );

    if (fd_entry < 0) {
        perror("open");
        goto main_cleanup;
    }

    ssize_t written = write_full(fd_entry, data, img_toc_p[idx].entry_size);
    if (written < 0) {
        perror("write");
        close(fd_entry);
        goto main_cleanup;
    }

    close(fd_entry);

    munmap(mem, filesize);
    close(fd);

    return 0;
main_cleanup:
    int err = errno;
    if (mem != MAP_FAILED) munmap(mem, filesize);
    if (fd > STDERR_FILENO) close(fd);
    if (err) return err;
    return -1;
}