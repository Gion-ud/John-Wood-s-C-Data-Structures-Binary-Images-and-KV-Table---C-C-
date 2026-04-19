#define _C_VECTOR_IMPLM
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vector.h"
#include "vector/algo.h"


int main(void) {
    Vector(int) v = Vector_create(int);
    assert(v);
    Vector_push(v, 3);
    Vector_push(v, 69);
    Vector_push(v, 727);
    Vector_push(v, 67);
    Vector_push(v, 911);
    Vector_push(v, 111);
    Vector_push(v, -1);
    Vector_push(v, 0);

    puts("before sort");
    for (__typeof__(v) it = Vector_begin(v); it != Vector_end(v); ++it) {
        printf("%d\n", *it);
    }
    putchar('\n');

    insertion_sort(v, Vector_length(v));
    puts("after sort");
    for (__typeof__(v) it = Vector_begin(v); it != Vector_end(v); ++it) {
        printf("%d\n", *it);
    }
    putchar('\n');

    printf("idxof(%d)=%d\n", 111, binary_search(v, Vector_length(v), 111));

    Vector_pop(v);
    Vector_pop(v);
    Vector_pop(v);
    puts("after pop");
    for (__typeof__(v) it = Vector_begin(v); it != Vector_end(v); ++it) {
        printf("%d\n", *it);
    }

    Vector_destroy(v);
    return 0;
}