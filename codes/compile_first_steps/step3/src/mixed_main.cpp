#include "mixed_a.hpp"
#include "mixed_b.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <number0> <number1>" << std::endl;
        return EXIT_FAILURE;
    }
    int a = atoi(argv[1]);
    int b = atoi(argv[2]);

    ma(a);
    mb(b);

    return EXIT_SUCCESS;
}