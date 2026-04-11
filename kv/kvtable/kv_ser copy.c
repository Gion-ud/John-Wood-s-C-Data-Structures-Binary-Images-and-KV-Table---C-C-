#include "kv_ser_priv.h"
#include <compute_crc32.h>
#include <posix_io.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "hash_index.h"

#define _FILE_OFFSET_BITS 64

int KVTable_serialise(
    KVTable    *_this,
    int         fd
) {
    if (_KVTable_validate(_this) < 0 || fd < 0 || !_this->kvcnt) return -1;

    KvFileHeader fhdr = {
        .fh_magic       = KV_FILE_MAGIC,
        .fh_version     = KV_FILE_VERSION,
        .fh_align       = _this->mem_arena.mem_align,
        .fh_fhdrsize    = sizeof(KvFileHeader),
        .fh_fftrsize    = sizeof(KvFileFooter),
        .fh_flags       = KVFILE_DEFAULT_FLAGS,
        .fh_kvcnt       = _this->kvcnt,
        .fh_tocoff      = sizeof(KvFileHeader),
        .fh_kvdatoff    = ALIGN_OFFSET(sizeof(KvFileHeader) + _this->kvcnt * sizeof(KvFileEntry), _this->mem_arena.mem_align),
        .fh_fftroff     = 0,    // TMP
    };
    KvFileFooter fftr = {
        .crc32          = 0, // reserved
        .ff_magic       = KV_EOF_MAGIC,
    };


    lseek(fd, ALIGN_OFFSET(sizeof(KvFileHeader), fhdr.fh_align), SEEK_SET);

    KvFileEntry *kvtbl_p = (KvFileEntry*)malloc(_this->kvcnt * sizeof(KvFileEntry));
    if (!kvtbl_p) return -1;

    for (ulong_t i = 0; i < _this->kvcnt; ++i) {
        printerrf("it%u;entrycnt=%u;hdrec=%u\n", i, _this->kvcnt, fhdr.fh_kvcnt);
        kvtbl_p[i].flags    = _this->kvtbl_p[i].entry_stat;
        kvtbl_p[i].key_len  = _this->kvtbl_p[i].key_len;
        kvtbl_p[i].key_off  = _this->kvtbl_p[i].key_off;
        kvtbl_p[i].val_len  = _this->kvtbl_p[i].val_len;
        kvtbl_p[i].val_off  = _this->kvtbl_p[i].val_off;
    }

    ssize_t ret = write_full(fd, kvtbl_p, _this->kvcnt * sizeof(KvFileEntry));
    if ((size_t)ret != _this->kvcnt * sizeof(KvFileEntry)) {
        free(kvtbl_p);
        return errno;
    }
    size_t mem_arena_end_cur = kvtbl_p[_this->kvcnt - 1].val_off + kvtbl_p[_this->kvcnt - 1].val_len;
    free(kvtbl_p);


    lseek(fd, fhdr.fh_kvdatoff, SEEK_SET);
    if (write_full(fd, _this->mem, mem_arena_end_cur) != (ssize_t)mem_arena_end_cur)
        return errno;

    off_t cur = lseek(fd, 0, SEEK_CUR);

    cur = ALIGN_OFFSET(cur, fhdr.fh_align);
    fhdr.fh_fftroff = cur;


    lseek(fd, 0, SEEK_SET);
    if (write_full(fd, &fhdr, sizeof(KvFileHeader)) != sizeof(KvFileHeader)) return errno;
    lseek(fd, 0, SEEK_SET);

    fftr.crc32 = compute_file_crc32_fildes(fd, 0, fhdr.fh_fftroff);
    lseek(fd, fhdr.fh_fftroff, SEEK_SET);

    if (
        write_full(fd, &fftr, sizeof(KvFileFooter)) != sizeof(KvFileFooter)
    )
        return errno;

    return 0;
}

extern KVTable *KVTable_create(
    size_t  kvcap,
    size_t  _mem_size
);

static inline uint32_t dword_next_pow2(uint32_t x) {
    if (!x) return 1;
    return 1u << (32 - __builtin_clz(x - 1));
}

#include <stdio.h>
KVTable *KVTable_create_load_from_file(int fd) {
    puts("KVTable_create_load_from_file");
    if (fd < 0) return NULL;

    struct stat st = {0};

    puts("fstat");
    if (fstat(fd, &st) < 0) return NULL;

    puts("mmap");
    void *f_mapped_mem_p = mmap(
        NULL,
        st.st_size,
        PROT_READ,
        MAP_SHARED | MAP_FILE,
        fd,
        0
    );
    if (f_mapped_mem_p == MAP_FAILED) return NULL;

    puts("header cast");
    KvFileHeader *fhdr_p = (KvFileHeader*)((byte_t*)f_mapped_mem_p);
    puts("validation");
    if (
        fhdr_p->fh_magic != KV_FILE_MAGIC ||
        fhdr_p->fh_fhdrsize != sizeof(KvFileHeader) ||
        fhdr_p->fh_fftrsize != sizeof(KvFileFooter) ||
        fhdr_p->fh_tocoff != fhdr_p->fh_fhdrsize ||
        fhdr_p->fh_kvdatoff - fhdr_p->fh_tocoff != fhdr_p->fh_kvcnt * sizeof(KvFileEntry) ||
        fhdr_p->fh_fftroff + fhdr_p->fh_fftrsize > (dword_t)st.st_size
    ) {
        munmap(f_mapped_mem_p, st.st_size);
        return NULL;
    }
    printf("fhdr_p->fh_fftroff=%u;%lu\n", fhdr_p->fh_fftroff, st.st_size);

    puts("footer cast");
    KvFileFooter *fftr_p = (KvFileFooter*)((byte_t*)f_mapped_mem_p + fhdr_p->fh_fftroff);
    puts("crc");
    ulong_t fcrc = fftr_p->crc32;// compute_mem_crc32(f_mapped_mem_p, fhdr_p->fh_fftroff);
    if (fftr_p->ff_magic != KV_EOF_MAGIC || fcrc != fftr_p->crc32) {
        printerrf("crc_src:0x%.8x;crc_current:0x%.8x\n", (ulong_t)fftr_p->crc32, (ulong_t)fcrc);
        printf("kvcnt=%u\n", fhdr_p->fh_kvcnt);
        munmap(f_mapped_mem_p, st.st_size);
        return NULL;
    }

    size_t mem_size_min = fhdr_p->fh_fftroff - fhdr_p->fh_kvdatoff;
    puts("data&&entry cast");
    void *f_kv_data_mem_p = (void*)((uintptr_t)f_mapped_mem_p + fhdr_p->fh_kvdatoff);
    KvFileEntry *f_toc_p = (KvFileEntry*)((byte_t*)f_mapped_mem_p + fhdr_p->fh_tocoff);

    puts("KVTable_create");
    KVTable *kvt_p = KVTable_create(dword_next_pow2(fhdr_p->fh_kvcnt), mem_size_min);
    if (!kvt_p) {
        munmap(f_mapped_mem_p, st.st_size);
        return NULL;
    }
    kvt_p->mem_arena.mem_cur = mem_size_min;

    puts("memcpy");
    memcpy(kvt_p->mem, f_kv_data_mem_p, mem_size_min);

    puts("loop");
    for (size_t i = 0; i < fhdr_p->fh_kvcnt; ++i) {
        printf("entry: %zu\n", i);
        kvt_p->kvtbl_p[i].key_len = f_toc_p[i].key_len;
        kvt_p->kvtbl_p[i].key_off = f_toc_p[i].key_off;
        kvt_p->kvtbl_p[i].val_len = f_toc_p[i].val_len;
        kvt_p->kvtbl_p[i].val_off = f_toc_p[i].val_off;
        kvt_p->kvtbl_p[i].entry_stat = f_toc_p[i].flags;

        printf("entry%zu hash\n", i);
        printf(
            "key=%.*s;len=%u;off=%u;arena_end_off=%zu\n",
            (int)f_toc_p[i].key_len, (char*)mem_arena_memcur_to_addr(&kvt_p->mem_arena, f_toc_p[i].key_off),
            f_toc_p[i].key_len,
            f_toc_p[i].key_off,
            kvt_p->mem_arena.mem_size
        );
        kvt_p->kvtbl_p[i].key_hash = ht_ops.hash32(
            mem_arena_memcur_to_addr(&kvt_p->mem_arena, f_toc_p[i].key_off),
            f_toc_p[i].key_len
        );
        ++kvt_p->kvcnt;
        ulong_t used_cnt = 0;
        printf("check\n");
        if (
            hash_index_table_insert(
                kvt_p->h_idx_tbl_p,
                kvt_p->kvcap,
                &used_cnt,
                kvt_p->kvtbl_p[i].key_hash,
                i
            ) < 0
        ) continue;
    }

    printf("munmap\n");
    munmap(f_mapped_mem_p, st.st_size);
    puts("ret");
    return kvt_p;
}

