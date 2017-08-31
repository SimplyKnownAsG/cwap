#pragma once

#include "cwap/Namespace.hpp"

#include <clang-c/Index.h>

namespace cwap {

    class Project : public Namespace {
    public:
        std::string filename;

        Project(std::string name);

        ~Project() = default;

        void parse(std::string filename);

        void parse(std::string filename, std::vector<std::string> clang_args);

        void write_yaml();
    };
}
