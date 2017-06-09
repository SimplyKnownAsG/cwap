#include "cwap/Project.hpp"

#include <clang-c/Index.h>

#include <iostream>
#include <string>
#include <vector>

void show_clang_version(void) {
    CXString version = clang_getClangVersion();
    std::cout << clang_getCString(version) << std::endl;
    clang_disposeString(version);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << argv[0] << "<project name> <file> [clang arguments]" << std::endl;
        exit(1);
    }

    show_clang_version();

    std::vector<std::string> args(argv + 3, argv + argc);

    cwap::Project proj(argv[1]);
    proj.parse(argv[2]);
    return 0;
}
