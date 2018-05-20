#include "cwap/Project.hpp"

#include <algorithm>
#include <clang-c/Index.h>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <string>
#include <vector>

namespace cwap {

    Project::Project(std::string name)
      : Namespace(name) {}

    void Project::parse(std::string filename) {
        std::vector<std::string> empty;
        return Project::parse(filename, empty);
    }

    void Project::parse(std::string filename, std::vector<std::string> clang_args) {
        std::vector<std::string> filenames{ filename };
        this->parse(filenames, clang_args);
    }

    void Project::parse(std::vector<std::string> filenames, std::vector<std::string> clang_args) {
        auto num_args = clang_args.size();

        CXIndex index = clang_createIndex(1, 1);

        std::vector<const char*> c_style_args;
        c_style_args.resize(clang_args.size(), nullptr);

        std::transform(clang_args.begin(),
                       clang_args.end(),
                       c_style_args.begin(),
                       [](std::string arg) { return arg.c_str(); });

        this->_sources.insert(filenames.begin(), filenames.end());

        for (auto filename : filenames) {
            // load source / header file
            CXTranslationUnit tu = clang_parseTranslationUnit(
              index,
              filename.c_str(),
              c_style_args.data(),
              num_args,
              NULL,
              0,
              CXTranslationUnit_PrecompiledPreamble | CXTranslationUnit_Incomplete);

            if (tu == NULL) {
                throw "Cannot create translation unit";
            }

            CXCursor cursor = clang_getTranslationUnitCursor(tu);
            struct ClangVisitorData wrapper {
                this, *this
            };
            clang_visitChildren(cursor, Project::VisitChildrenCallback, &wrapper);

            clang_disposeTranslationUnit(tu);
        }
        clang_disposeIndex(index);
    }

    void Project::write_yaml() {
        std::ofstream yaml(this->name + ".yaml");
        this->dump_yaml(yaml);
    }

    const std::unordered_set<std::string> Project::sources() const {
        return this->_sources;
    }
}
