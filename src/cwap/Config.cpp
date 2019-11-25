#include "cwap/Config.hpp"
#include "cwap/Location.hpp"
#include "cwap/internal/ClangVisitor.hpp"
#include "cwap/internal/ConvenientClang.hpp"
#include "cwap/internal/Factory.hpp"

#include "yaml-cpp/yaml.h"

#include "globpp/globpp.hpp"

#include <clang-c/Index.h>

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace cwap {
    static vector<string> initialize_inc_dirs();

    Config::Config() {
        this->_std_inc_dirs = initialize_inc_dirs();

        auto exec_path = globpp::dirname(globpp::get_current_executable());
        auto pattern = globpp::join(exec_path, "*.yaml");

        for (auto fname : globpp::glob(pattern)) {
            update_config(this, fname);
        }

        for (auto& kv : this->_type_renames) {
            std::cout << "RENAME: " << kv.first << " -> " << kv.second << "\n";
        }
    }

    Config& Config::GetConfig() {
        // XXX: this will probably ruin someone's day.
        //      Should allow different configs
        static Config conf;
        return conf;
    }

    vector<string> const& Config::get_std_inc_dirs() {
        return _std_inc_dirs;
    }

    static vector<string> initialize_inc_dirs() {
        vector<string> std_inc_dirs;

        // XXX: would be really, really nice to not need a temp file
        std::system("clang++ -E -v -xc++ /dev/null > .cwap-temp-file.out 2>&1");

        {
            std::ifstream infile(".cwap-temp-file.out");
            std::string line;

            // look for /^#include <...> starts here/
            while (std::getline(infile, line)) {
                if (line.find("#include <") == 0) {
                    break;
                }
            }

            // read lines until they don't start with a couple spaces.
            while (std::getline(infile, line)) {
                if (line.find(" /") == 0) {
                    std_inc_dirs.push_back("-isystem");
                    std_inc_dirs.push_back(line.substr(1, line.find(" ", 1) - 1));
                } else {
                    break;
                }
            }
        }

        if (std_inc_dirs.size() == 0) {
            throw std::runtime_error(
                    "Could not get clang++ std include directories.\n"
                    "This is necessary because the linked clang libraries use reference\n"
                    "paths instead of fully-qualified paths, and they are in reference to the\n"
                    "current executable. See `.cwap-temp-file.out`, which is the output of\n"
                    "  $ clang++ -v -xc++ /dev/null");
        }

        std::remove(".cwap-temp-file.out");
        return std_inc_dirs;
    }

    struct RenameThisData {
        unordered_map<string, string>& type_renames;
        std::string const& new_name;
    };

    CXChildVisitResult find_RENAME_THIS(CXCursor cursor,
                                        CXCursor parent,
                                        CXClientData client_data) {
        (void)parent; // suppress -Wunused-parameter

        Location location = Location::Create(cursor);
        if (location.file_name != ".cwap-temp-file.hpp") {
            return CXChildVisit_Continue;
        }

        CXCursorKind cursor_kind = clang_getCursorKind(cursor);
        if (!clang_isDeclaration(cursor_kind)) {
            return CXChildVisit_Continue;
        }

        std::string name = internal::get_name(cursor);

        if (name == "RENAME_THIS") {
            struct RenameThisData* rtd = ((struct RenameThisData*)client_data);
            const std::string& new_name = rtd->new_name;
            std::string type_name = internal::get_type_name(cursor);
            rtd->type_renames[type_name] = new_name;
            return CXChildVisit_Break;
        }

        return CXChildVisit_Continue;
    }

    void update_config(Config* that, std::string filename) {
        YAML::Node config = YAML::LoadFile(filename);
        YAML::Node type_renames = config["type renames"];
        if (type_renames) {
            for (YAML::const_iterator it = type_renames.begin(); it != type_renames.end(); ++it) {
                std::string new_name = it->first.as<std::string>();
                std::string text = it->second.as<std::string>();

                // XXX: would be really nice to not need a temp file. I believe that llvm C++ can
                // read a buffer instead of a file (that's how one would create auto-compelte tools)
                std::ofstream temp_file(".cwap-temp-file.hpp");
                temp_file << text;
                temp_file.close();

                vector<const char*> c_style_args(that->get_std_inc_dirs().size());
                /* char const** c_style_args = new char const*[get_std_inc_dirs().size()]; */
                std::transform(that->get_std_inc_dirs().begin(),
                               that->get_std_inc_dirs().end(),
                               c_style_args.begin(),
                               [](std::string const& arg) { return arg.c_str(); });

                CXIndex index = clang_createIndex(1, 1);
                CXTranslationUnit tu =
                        clang_parseTranslationUnit(index,
                                                   ".cwap-temp-file.hpp",
                                                   c_style_args.data(),
                                                   c_style_args.size(),
                                                   NULL,
                                                   0,
                                                   CXTranslationUnit_SkipFunctionBodies |
                                                           CXTranslationUnit_PrecompiledPreamble);
                std::remove(".cwap-temp-file.hpp");

                CXCursor cursor = clang_getTranslationUnitCursor(tu);
                struct RenameThisData wrapper {
                    that->_type_renames, new_name
                };
                clang_visitChildren(cursor, find_RENAME_THIS, &wrapper);

                clang_disposeTranslationUnit(tu);
                clang_disposeIndex(index);
            }
        }
    }

    unordered_map<string, string> const& Config::get_type_renames() {
        return this->_type_renames;
    }

}
