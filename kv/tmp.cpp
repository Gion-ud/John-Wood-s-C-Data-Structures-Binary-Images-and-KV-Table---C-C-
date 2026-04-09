#include <iostream>
#include <new>
#include <cstddef>
#include <stdexcept>

typedef unsigned char buffer[4096];

int main() {
    throw std::runtime_error("nice try");
    return 0;
}