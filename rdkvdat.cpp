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

#include "dctr.h"

char buffer[512] = {0};


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

    std::vector<cstr> key_vec;
    std::vector<cstr> val_vec;

    cstr key_sz;
    cstr val_sz;

    char *line_p = (char*)line_buf;
    size_t line_len = sizeof(line_buf) - 1;

    std::cout << "\t-- KV Table --\n" << "\n";
    KVPairView kvpv = {0};
    for (auto i = 0zu; i < kvt_p->size(); ++i) {
        const KVPairView *kv_p = kvt_p->get_kv_view(kvpv, i);
        if (!kv_p) continue;
        printf(
            "[%zu]\tkey=\"%.*s\"\n\tval=\"%.*s\"\n\n",
            i,
            (int)kv_p->key_len,(char*)kv_p->key_ptr,
            (int)kv_p->val_len,(char*)kv_p->val_ptr
        );
    }


    kvt_p->~KVTable();

    return 0;
}
