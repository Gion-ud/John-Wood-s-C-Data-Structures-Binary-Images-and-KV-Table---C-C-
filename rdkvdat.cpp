extern "C" {
#include <tlv/tlv.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
}
#include <iostream>
#include <new>
#include <string>
#include <vector>
#include <kvtable/kvtable.hpp>
#include <cstr.h>

#define mainarg_this_prog   argv[0]
#define mainarg_kvfilename  argv[1]
#define mainargc            2

#include "buffer.h"

static char line_buf[4096] = {0};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: <this prog name> <kv capacity>\n";
        return -1;
    }

    byte_t kvt_mem[sizeof(kv::KVTable)] = {0};
    kv::KVTable *kvt_p = new (kvt_mem) kv::KVTable();

    puts("before load");
    if (kvt_p->load_from_file(mainarg_kvfilename)) {
        std::cerr << "kvt_p->load_from_file() failed\n";
        kvt_p->~KVTable();
        return -1;
    }
    puts("after load");

    char *line_p = static_cast<char*>(line_buf);
    size_t line_len = sizeof(line_buf) - 1;

    std::cout << "\t-- KV Table --\n" << "\n";
    KVPairView kvpv{};
    for (size_t i{0}; i < kvt_p->size(); ++i) {
        const KVPairView *kv_p = kvt_p->get_kv_view(kvpv, i);
        if (!kv_p) continue;
        printf(
            "[%zu]\tkey=\"%.*s\"\n\tval=\"%.*s\"\n\n",
            i,
            static_cast<int>(kv_p->key_len),static_cast<char*>(kv_p->key_ptr),
            static_cast<int>(kv_p->val_len),static_cast<char*>(kv_p->val_ptr)
        );
    }
    std::cout << "\n\n";
    
    LPBuffer out_val{};
    while(1) {
        std::cout << "Enter key to retrieve value:\n";
        std::cin.getline(line_p, line_len + 1);
        line_p = line_strip(line_p);
        if (!line_p || !strlen(line_p)) break;
        if (
            kvt_p->lookup(
                *cstr_to_lpbuf_rdonly(&out_val, line_p),
                out_val
            ) < 0
        ) {
            std::cerr << "key not found\n";
            continue;
        };
        printf(
            "kvt[\"%s\"]=\"%.*s\"\n",
            line_p,
            static_cast<int>(out_val.len), static_cast<char*>(out_val.data)
        );
    }

    kvt_p->~KVTable();

    return 0;
}
