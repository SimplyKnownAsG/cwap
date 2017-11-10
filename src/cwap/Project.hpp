#pragma once

#include "cwap/Namespace.hpp"

#include <clang-c/Index.h>

#include <string>
#include <unordered_set>

namespace cwap {

    class Project : public Namespace {
    private:
        std::unordered_set<std::string> _sources;

    public:
        Project(std::string name);

        ~Project() = default;

        const std::unordered_set<std::string> sources() const;

        void parse(std::string filename);

        void parse(std::string filename, std::vector<std::string> clang_args);

        void parse(std::vector<std::string> filenames, std::vector<std::string> clang_args);

        void write_yaml();
    };
}
