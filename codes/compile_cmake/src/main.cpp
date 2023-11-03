#include "particle.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <number>" << std::endl;
        return EXIT_FAILURE;
    }
    int a = atoi(argv[1]);
    use_v3(a);

    return EXIT_SUCCESS;
}