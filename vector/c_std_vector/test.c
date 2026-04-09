#include <stdio.h>
#include <time.h>
#include <string.h>
#include "c_vector.h"

// A "Heavy" struct to test stride and alignment
typedef struct {
    size_t id;
    double data[8]; 
    char label[16];
} HeavyNode;

void print_report(const char* test_name, double time_taken, size_t ops) {
    printf("[PERF] %-25s: %.4f sec (%zu ops/sec)\n", 
           test_name, time_taken, (size_t)(ops / (time_taken > 0 ? time_taken : 1)));
}

int main() {
    printf("--- C23 High-Performance Vector Stress Test ---\n\n");

    // --- TEST 1: RAW THROUGHPUT (1 Million Pushes) ---
    auto v_int = new_Vector(int);
    clock_t start = clock();
    const int iterations = 1000000;

    for (int i = 0; i < iterations; i++) {
        Vector_push(int, v_int, &i);
    }
    
    double time_push = (double)(clock() - start) / CLOCKS_PER_SEC;
    print_report("1M Integer Pushes", time_push, iterations);

    // --- TEST 2: ITERATOR TRAVERSAL SPEED ---
    start = clock();
    long long sum = 0;
    for (auto it = Vector_begin(int, v_int); it != Vector_end(int, v_int); ++it) {
        sum += *it;
    }
    double time_iter = (double)(clock() - start) / CLOCKS_PER_SEC;
    print_report("1M Iterator Sum", time_iter, iterations);
    assert(sum == (long long)iterations * (iterations - 1) / 2);

    // --- TEST 3: HEAVY STRUCT & REALLOC TORTURE ---
    // This tests if your vector handles non-trivial memory strides
    auto v_heavy = new_Vector(HeavyNode);
    start = clock();
    
    for (size_t i = 0; i < 100000; i++) {
        HeavyNode node = { .id = i, .data = { [0] = (double)i } };
        strcpy(node.label, "STRESS_TEST");
        Vector_push(HeavyNode, v_heavy, &node);
    }
    
    double time_heavy = (double)(clock() - start) / CLOCKS_PER_SEC;
    print_report("100k Heavy Structs", time_heavy, 100000);

    // --- TEST 4: ALL API COVERAGE (The "Kitchen Sink") ---
    printf("\n[Check] Validating All API States...\n");
    
    // Test: Vector_get & Vector_data
    auto second_node = Vector_get(HeavyNode, v_heavy, 1);
    assert(second_node->id == 1);
    assert(Vector_data(HeavyNode, v_heavy) == Vector_begin(HeavyNode, v_heavy));

    // Test: Vector_reserve (Expansion)
    size_t old_cap = Vector_capacity(v_heavy);
    Vector_reserve(v_heavy, old_cap * 2);
    assert(Vector_capacity(v_heavy) >= old_cap * 2);

    // Test: Vector_clear vs Vector_empty
    Vector_clear(v_heavy);
    assert(Vector_size(v_heavy) == 0);
    assert(Vector_empty(v_heavy) == true);

    // Test: Pop safety on cleared vector
    assert(Vector_pop(v_heavy) == false);

    // --- CLEANUP ---
    delete_Vector(v_int);
    delete_Vector(v_heavy);

    printf("\n--- All Tests Completed Successfully ---\n");
    return 0;
}