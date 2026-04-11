#ifndef KV_TABLE_HPP
#define KV_TABLE_HPP

/*
IMPORTANT:
    DO NOT USE ASSIGN OPERATOR for an object UNLESS 
    YOU ARE ASSIGNING TO obj.move_out() for ownership
    transfer or obj.copy() for making a new copy that's
    independent from the original object
*/

extern "C" {
#include "kvtable.h"
#include "kv_ser.h"
#include <stddef.h>
#include <sys/types.h>
#include <fcntl.h>
#include <posix_io.h>
#include <stdio.h>
#include <errno.h>
}
#include <stdexcept>
#include <iostream>

typedef struct _kvtblobj *KVTableHandle;

#define MEM_SIZE (64 * 1024U)

#ifndef _WIN32
#define O_BINARY 0x8000
#endif

namespace kv {

class KVTable {
private:
    KVTableHandle kvt_h;
public:
    KVTable() noexcept : kvt_h(nullptr) {}
    ssize_t create(size_t capacity) noexcept {
        if (this->kvt_h) KVTable_destroy(this->kvt_h);
        this->kvt_h = KVTable_create(capacity, MEM_SIZE);
        if (!this->kvt_h) {
            std::cerr << "KVTable_create failed\n";
            return -1;
        }
        return (ssize_t)capacity;
    }
    size_t size() noexcept {
        return KVTable_size(this->kvt_h);
    }
    size_t capacity() noexcept {
        return KVTable_capacity(this->kvt_h);
    }
    const KVPairView *get_kv_view(KVPairView &out_kvpv, size_t idx) noexcept {
        return KVTable_get(this->kvt_h, &out_kvpv, idx);
    }

    int load_from_file(const char *filename_p) noexcept {
        puts("KVTable::load_from_file");
        if (this->kvt_h) KVTable_destroy(this->kvt_h);
        puts("open");
        int fd = open(filename_p, O_RDONLY);
        if (fd < 0) {
            perror("open failed");
            return errno;
        }
        puts("KVTable_create_load_from_file");
        this->kvt_h = KVTable_create_load_from_file(fd);
        if (!this->kvt_h) {
            std::cerr << "KVTable_create failed\n";
            return -1;
        }
        puts("close");
        close(fd);
        return 0;
    }
    bool insert(const LPBuffer &key, const LPBuffer &val) noexcept {
        KVTableHandle kvt_tmp_h = KVTable_insert(
            this->kvt_h, const_cast<LPBuffer*>(&key), const_cast<LPBuffer*>(&val));
        if (!kvt_tmp_h) {
            std::cerr << "KVTable_insert failed\n";
            return false;
        }
        this->kvt_h = kvt_tmp_h;
        return true;
    }
    hidx_t lookup(const LPBuffer &key, LPBuffer &out_val) noexcept {
        hidx_t idx = KVTable_lookup(
            this->kvt_h, const_cast<LPBuffer*>(&key), &out_val
        );
        if (idx < 0) {
            std::cerr << "KVTable_lookup failed\n";
            return idx;
        }
        return idx;
    }
    bool compact() noexcept {
        KVTableHandle kvt_tmp_h = KVTable_compact(this->kvt_h);
        if (!kvt_tmp_h) {
            std::cerr << "KVTable_compact failed\n";
            return false;
        }
        this->kvt_h = kvt_tmp_h;
        return true;
    }
    int serialise(const char *filename) {
        int fd = open(filename, O_RDWR | O_CREAT | O_BINARY, 0644);
        if (fd < 0) {
            perror("kv::KVTable::serialise: open failed");
            return errno;
        }
        ftruncate(fd, 0);
        int ret = KVTable_serialise(this->kvt_h, fd);
        if (ret) {
            std::cerr << "KVTable_serialise failed\n";
            close(fd);
            return ret;
        }
        close(fd);
        return 0;
    }
    KVTable &operator=(const KVTable &obj) = delete;
    KVTable(const KVTable &obj) = delete;
    ~KVTable() noexcept {
        KVTable_destroy(this->kvt_h);
    }

};


}




#endif