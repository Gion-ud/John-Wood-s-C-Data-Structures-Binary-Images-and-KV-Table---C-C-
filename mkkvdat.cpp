extern "C" {
#include <tlv/tlv.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
}
#include <iostream>
#include <new>
#include <string>
#include <vector>
#include <kvtable/kvtable.hpp>
#include <cstr.h>

#define mainarg_this_prog       argv[0]
#define mainarg_kvcap_cstr      argv[1]
#define mainarg_filename_cstr   argv[2]
#define mainargc            2

#include "buffer.h"

static char line_buf[4096] = {0};


int main(int argc, char *argv[]) {
    if (argc != 2 && argc != 3) {
        std::cerr << "Usage: <this prog name> <kv capacity> (<kv filename>)\n";
        return -1;
    }
    char *end_p = NULL;
    long kvcap = strtol(mainarg_kvcap_cstr, &end_p, 10);
    if (end_p[0] != '\0') {
        std::cerr << "strtol failed: Invalid kv capacity\n";
        return -1;
    }

    void *kvt_mem = operator new(sizeof(kv::KVTable), std::nothrow);
    if (!kvt_mem) {
        std::cerr << "new failed\n";
        return -1;
    }
    kv::KVTable *kvt_p = new (kvt_mem) kv::KVTable();

    if (kvt_p->create(kvcap) < 0) {
        std::cerr << "kvt_p->create() failed\n";
        kvt_p->~KVTable();
        operator delete(kvt_mem);
        return -1;
    }

    std::vector<cstr> key_vec;
    std::vector<cstr> val_vec;

    cstr key_sz;
    cstr val_sz;

    char *line_p = static_cast<char*>(line_buf);
    size_t line_len = sizeof(line_buf) - 1;
    for (auto i = 0; i < kvcap; ++i) {
        std::cout << '[' << i << "]\nEnter key: ";
        std::cin.getline(line_p, line_len + 1);
        key_sz.copy_from_sz(line_p);
        key_sz.strip();
        key_vec.push_back(key_sz.move_out());

        std::cout << "Enter Val: ";
        std::cin.getline(line_p, line_len + 1);
        line_p = line_strip(line_p);
        val_sz.copy_from_sz(line_p);
        val_vec.push_back(val_sz.move_out());

        std::cout << '\n';
    }

    std::cout << " -- KV Table --\n";
    for (auto i = 0; i < kvcap; ++i) {
        const char *key_sz_p = key_vec[i].data();
        const char *val_sz_p = val_vec[i].data();
        if (!key_sz_p || !val_sz_p) continue;
        std::cout << "[" <<  i << "]\tkey=\"" << key_sz_p << "\"\n";
        std::cout << "\tval=\"" << val_sz_p << "\"\n\n";
    }

    for (auto i = 0; i < kvcap; ++i) {
        LPBuffer key, val;
        const char *key_sz_p = key_vec[i].data();
        const char *val_sz_p = val_vec[i].data();
        if (!key_sz_p || !val_sz_p) continue;
        if (
            !kvt_p->insert(
                cstr_to_lpbuf_rdonly(&key, key_sz_p),
                cstr_to_lpbuf_rdonly(&val, val_sz_p)
            )
        ) {
            std::cerr << "kvt_p->insert() failed at iteration " << i << "\n";
            continue;
        }
    }
    char *filename_cstr_p = nullptr;

    if (mainarg_filename_cstr && *mainarg_filename_cstr) {
        filename_cstr_p = mainarg_filename_cstr;
    } else {
        filename_cstr_p = static_cast<char*>(line_buf);
        snprintf(
            filename_cstr_p, sizeof(line_buf),
            "kvdat_%.8llx.bin",
            (qword_t)time(NULL)
        );  
    }

    if (kvt_p->serialise(filename_cstr_p)) {
        std::cerr << "kvt_p->serialise() failed\n";
    }

    kvt_p->~KVTable();
    operator delete(kvt_mem);

    return 0;
}
