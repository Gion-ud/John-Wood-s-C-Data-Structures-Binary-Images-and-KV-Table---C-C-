#include "bipf/imgpch.h"

#ifndef O_BINARY
#define O_BINARY 0x8000
#endif

const char *res_filename_arr[] = {
    "res/images/121200433_p0.jpg",
    "res/images/121200433_p1.jpg",
    "res/images/121200433_p2.jpg",
    "res/images/126711989_p0.jpg",
    "res/images/126711989_p1.jpg",
    "res/images/126711989_p2.jpg",
};

static const char *filename = "assets.img";
static const size_t entrycnt =
    sizeof(res_filename_arr) / sizeof(res_filename_arr[0]);

static int ret = 0;

int main() {
    int fd_img = open(filename, O_RDWR | O_CREAT | O_BINARY, 0644);
    if (fd_img < 0) {
        perror("open");
        goto main_cleanup;
    }

    size_t memsize = IMG_DATA_OFF(entrycnt);
    if (ftruncate(fd_img, memsize) < 0) {
        perror("ftruncate");
        goto main_cleanup;
    }

    void *mem = malloc(memsize);
    if (!mem) {
        printerrf("malloc\n");
        goto main_cleanup;
    }
    print_dbg_msg("base_addr: 0x%.16llx\n", (addr_t)mem);

    IMG_FILE_OBJECT img = {0};
    ret = IMG_FILE_OBJECT_init(&img, fd_img, mem, memsize, entrycnt, FLAG_NONE);
    if (ret < 0) {
        printerrf("IMG_FILE_OBJECT_init failed: %s\n", img_strerror(ret));
        goto main_cleanup;
    }

    int asset_fd = -1;
    void *asset_mem = NULL;
    struct stat st;

    for (size32_t i = 0; i < entrycnt; ++i) {
        printf("%.4u,%s\n", i, res_filename_arr[i]);

        asset_fd = open(res_filename_arr[i], O_RDONLY | O_BINARY);
        if (asset_fd < 0) {
            perror("open");
            goto loop_failed;
        }
        fstat(asset_fd, &st);
        asset_mem = mmap(
            NULL,
            st.st_size,
            PROT_READ,
            MAP_SHARED | MAP_FILE,
            asset_fd,
            0
        );
        if (asset_mem == MAP_FAILED) {
            perror("mmap");
            goto loop_failed;
        }
        ret = IMG_FILE_write_entry_seqio(
            &img,
            IMG_ENTRY_FILE,
            FLAG_NONE,
            st.st_size,
            asset_mem
        );
        if (ret < 0) {
            printerrf("failed to write entry: %s\n", img_strerror(ret));
            close(asset_fd);
            munmap(asset_mem, st.st_size);
            goto loop_failed;
        }
        munmap(asset_mem, st.st_size);
        close(asset_fd);
        continue;
    loop_failed:
        if (asset_fd > 2) close(asset_fd);
        if ((ptrdiff_t)asset_mem > 0) munmap(asset_mem, st.st_size);
        goto main_cleanup;
    }

    ret = IMG_FILE_flush_seqio(&img);
    if (ret < 0) {
        printerrf("IMG_FILE_flush: %s\n", img_strerror(ret));
        goto main_cleanup;
    }

    IMG_FILE_OBJECT_deinit(&img);
    munmap(mem, memsize);
    close(fd_img);

    return 0;
main_cleanup:
    int err = errno;
    if ((ptrdiff_t)mem > 0) munmap(mem, memsize);
    if (fd_img > STDERR_FILENO) close(fd_img);
    if (err) return err;
    return ret;
}