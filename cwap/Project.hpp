#pragma once

#include "cwap/Namespace.hpp"

#include <clang-c/Index.h>

namespace cwap {

    class Project : public Namespace {
    public:
        Project(std::string name);

        void parse(std::string filename);

        void parse(std::string filename, std::vector<std::string> clang_args);
    };
}
