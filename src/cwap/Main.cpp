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
    try {
        if (argc <= 2) {
            std::cout << argv[0] << " <project name> <file> [clang arguments]" << std::endl;
            exit(1);
        }

        show_clang_version();

        cwap::Project proj(argv[1]);

        std::vector<std::string> args(argv + 3, argv + argc);
        proj.parse(argv[2], args);

        proj.write_yaml();

        return 0;
    } catch (std::exception& exc) {
        std::cerr << "Program terminating unsuccessfully. exception message:" << std::endl;
        std::cerr << exc.what() << std::endl;
    }
    return 1;
}
