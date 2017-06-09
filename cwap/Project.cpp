#include "cwap/Project.hpp"

#include <clang-c/Index.h>

namespace cwap {

    Project::Project(std::string name)
      : CwapNamespace(name) {}

    void Project::parse(std::string filename) {
        std::vector<std::string> empty;
        return Project::parse(filename, empty);
    }

    void Project::parse(std::string filename, std::vector<std::string> clang_args) {
        auto numArgs = clang_args.size();

        CXIndex index = clang_createIndex(1, 1);

        std::vector<const char*> c_style_args;
        std::transform(clang_args.begin(),
                       clang_args.end(),
                       c_style_args.begin(),
                       [](std::string arg) { return arg.c_str(); });

        // load source / header file
        CXTranslationUnit tu = clang_parseTranslationUnit(index,
                                                          filename.c_str(),
                                                          c_style_args.data(),
                                                          numArgs,
                                                          NULL,
                                                          0,
                                                          CXTranslationUnit_PrecompiledPreamble |
                                                            CXTranslationUnit_Incomplete);
        if (tu == NULL) {
            throw "Cannot create translation unit";
        }

        CXCursor cursor = clang_getTranslationUnitCursor(tu);
        clang_visitChildren(cursor, Project::VisitChildrenCallback, this);

        clang_disposeTranslationUnit(tu);
        clang_disposeIndex(index);
    }
}
