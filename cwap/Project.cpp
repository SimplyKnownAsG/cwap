#include "cwap/Project.hpp"

#include <algorithm>
#include <clang-c/Index.h>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <string>
#include <vector>

#ifndef _WIN32
#include <execinfo.h>
#include <ucontext.h>
#include <unistd.h>
#endif

namespace cwap {

#ifndef _WIN32
    void handler(int sig) {
        void* array[10];
        size_t size;
        // get void*'s for all entries on the stack
        size = backtrace(array, 10);

        // print out all the frames to stderr
        std::cerr << "Error: signal " << sig << std::endl;
        backtrace_symbols_fd(array, size, STDERR_FILENO);
        exit(1);
    }

    int install_handler() {
        /* Install our signal handler */

        signal(SIGSEGV, handler);
        signal(SIGUSR1, handler);

        return 1;
    }

    static int installed_handler = install_handler();
#endif

    Project::Project(std::string name)
      : Namespace(name) {
        this->project = this;
    }

    void Project::parse(std::string filename) {
        std::vector<std::string> empty;
        return Project::parse(filename, empty);
    }

    void Project::parse(std::string filename, std::vector<std::string> clang_args) {
#ifndef _WIN32
        if (installed_handler != 1) {
            installed_handler = install_handler();
        }
#endif
        this->filename = filename;
        auto numArgs = clang_args.size();

        CXIndex index = clang_createIndex(1, 1);

        std::vector<const char*> c_style_args;
        c_style_args.resize(clang_args.size(), nullptr);

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

    void Project::write_yaml() {
        std::ofstream yaml(this->name + ".yaml");
        this->dump_yaml(yaml);
    }
}
