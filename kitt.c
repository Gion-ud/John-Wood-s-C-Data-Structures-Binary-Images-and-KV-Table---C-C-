#define _C_VECTOR_IMPLM
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kv/key_index_table/kidxtbl.h"
#include <mem/arena_alloc.h>
#include <assert.h>
#include "vector.h"

#define ALLOC_MEM_ALIGN 4
#define MEM_SIZE 		(1024 * 1024ULL)
#define KITCAP 			64


typedef char *cstr_t;

#define lookup KeyIndexTable_lookup
#define get_key KeyIndexTable_get_key
#define insert KeyIndexTable_insert
#define kit_sizeof KeyIndexTable_size


#define put_kv(kit_p, val_vec, key_cstr, val) \
	do {\
		insert(kit_p, key_cstr);\
		Vector_push(val_vec, (__typeof__(*val_vec))val);\
	} while (0)

#define print_cstr_kvpair(kit, val_cstr_vec, key_cstr_p) \
	do {\
		kidx_t kidx = lookup(&kit, key_cstr_p);\
		printf("%s => %s\n", key_cstr_p, kidx >= 0 ? val_cstr_vec[kidx] : "(null)");\
	} while (0)

int main() {
    void *mem = malloc(MEM_SIZE);
	assert(mem);
	memset(mem, 0, MEM_SIZE);

    MemArena arena = {0};
	struct arena_alloc alloc = {0};
	arena_alloc_init(&alloc, &arena, mem, MEM_SIZE, ALLOC_MEM_ALIGN);

    KeyIndexTable kit = {0};
	size_t mem_cur_mark = alloc.getcur(alloc.alloc_handle);
	int ret = KeyIndexTable_init(&kit, KITCAP, &alloc);
	assert(ret >= 0);

	Vector(cstr_t) v = Vector_create(cstr_t);
	assert(v);

	put_kv(&kit, v, "open", "OPEN");
	put_kv(&kit, v, "read", "READ");
	put_kv(&kit, v, "write", "WRITE");
	put_kv(&kit, v, "close", "CLOSE");
	put_kv(&kit, v, "mmap", "MMAP");
	
	puts("");
	for (size_t i = 0; i < kit_sizeof(&kit); ++i) {
		printf("%.4zu: %s\n", i, get_key(&kit, i));
	}
	puts("");
	for (size_t i = 0; i < kit_sizeof(&kit); ++i) {
		printf("%.4zu: %s\n", i, get_key(&kit, i));
	}
	puts("");

	print_cstr_kvpair(kit, v, "open");
	print_cstr_kvpair(kit, v, "opened");
	print_cstr_kvpair(kit, v, "open ");
	print_cstr_kvpair(kit, v, " open");
	print_cstr_kvpair(kit, v, "close");
	print_cstr_kvpair(kit, v, "mmap");
	print_cstr_kvpair(kit, v, " mmap ");

	Vector_destroy(v);
	KeyIndexTable_deinit(&kit, mem_cur_mark);

	arena_alloc_deinit(&alloc); 
    free(mem);

    return 0;
}