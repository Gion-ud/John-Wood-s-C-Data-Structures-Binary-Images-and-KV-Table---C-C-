#define _C_VECTOR_IMPLM
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vector.h"

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

    for (__typeof__(v) it = Vector_begin(v); it != Vector_end(v); ++it) {
        printf("%d\n", *it);
    }
    Vector_pop(v);
    Vector_pop(v);
    Vector_pop(v);
    puts("");

    for (__typeof__(v) it = Vector_begin(v); it != Vector_end(v); ++it) {
        printf("%d\n", *it);
    }

    for (__typeof__(v) it = Vector_end(v) - 1; it != Vector_begin(v) - 1; --it) {
        printf("%d\n", *it);
    }

    Vector_destroy(v);
    return 0;
}