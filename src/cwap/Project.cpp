#include "cwap/Project.hpp"
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

    static std::vector<string> const& get_std_inc_dirs() {
        static std::vector<string> std_dirs;

        if (std_dirs.size() == 0) {
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
                        std_dirs.push_back("-isystem");
                        std_dirs.push_back(line.substr(1, line.find(" ", 1) - 1));
                    } else {
                        break;
                    }
                }
            }

            if (std_dirs.size() == 0) {
                throw std::runtime_error(
                        "Could not get clang++ std include directories.\n"
                        "This is necessary because the linked clang libraries use reference\n"
                        "paths instead of fully-qualified paths, and they are in reference to the\n"
                        "current executable. See `.cwap-temp-file.out`, which is the output of\n"
                        "  $ clang++ -v -xc++ /dev/null");
            }

            std::remove(".cwap-temp-file.out");
        }

        return std_dirs;
    }

    Project::Project(std::string name)
      : Namespace("", name) {
        auto exec_path = globpp::dirname(globpp::get_current_executable());
        auto pattern = globpp::join(exec_path, "*.yaml");

        for (auto fname : globpp::glob(pattern)) {
            this->process_options(fname);
        }
    }

    struct RenameThisData {
        Project& project;
        const std::string& new_name;
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
            Project& project = ((struct RenameThisData*)client_data)->project;
            const std::string& new_name = ((struct RenameThisData*)client_data)->new_name;
            std::string type_name = internal::get_type_name(cursor);
            project.type_renames[type_name] = new_name;
            return CXChildVisit_Break;
        }

        return CXChildVisit_Continue;
    }

    void Project::process_options(std::string filename) {
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

                std::vector<const char*> c_style_args(get_std_inc_dirs().size());
                /* char const** c_style_args = new char const*[get_std_inc_dirs().size()]; */
                std::transform(get_std_inc_dirs().begin(),
                               get_std_inc_dirs().end(),
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
                    *this, new_name
                };
                clang_visitChildren(cursor, find_RENAME_THIS, &wrapper);

                clang_disposeTranslationUnit(tu);
                clang_disposeIndex(index);
            }
        }
    }

    void Project::parse(std::string filename) {
        std::vector<std::string> empty;
        return Project::parse(filename, empty);
    }

    void Project::parse(std::string filename, std::vector<std::string> clang_args) {
        std::vector<std::string> filenames{ filename };
        this->parse(filenames, clang_args);
    }

    void Project::parse(std::vector<std::string> filenames, std::vector<std::string> clang_args) {
        CXIndex index = clang_createIndex(1, 1);

        std::vector<const char*> c_style_args(clang_args.size() + get_std_inc_dirs().size());
        std::transform(get_std_inc_dirs().begin(),
                       get_std_inc_dirs().end(),
                       c_style_args.begin(),
                       [](std::string const& arg) { return arg.c_str(); });
        std::transform(clang_args.begin(),
                       clang_args.end(),
                       c_style_args.begin() + get_std_inc_dirs().size(),
                       [](std::string const& arg) { return arg.c_str(); });

        this->_sources.insert(filenames.begin(), filenames.end());

        for (auto filename : filenames) {
            // load source / header file
            // this time we need function bodies (no CXTranslationUnit_SkipFunctionBodies)
            // because that is how we can get the content for templates
            CXTranslationUnit tu = clang_parseTranslationUnit(
                    index,
                    filename.c_str(),
                    c_style_args.data(),
                    c_style_args.size(),
                    NULL,
                    0,
                    CXTranslationUnit_PrecompiledPreamble | CXTranslationUnit_Incomplete);

            if (tu == NULL) {
                clang_disposeTranslationUnit(tu);
                throw std::invalid_argument("Cannot create translation unit");
            }

            CXCursor cursor = clang_getTranslationUnitCursor(tu);
            internal::Factory factory(this);
            struct internal::ClangVisitor::ClangVisitorData data {
                &factory, this, this, nullptr
            };
            clang_visitChildren(cursor, internal::ClangVisitor::VisitChildrenCallback, &data);

            clang_disposeTranslationUnit(tu);
        }
        clang_disposeIndex(index);
    }

    void Project::write_header(std::ostream& stream, std::string indent) const {

        for (auto func : this->functions()) {
            func->write_header(stream, indent);
        }

        for (auto name_variable : this->variables()) {
            stream << indent << name_variable.second->cwap_type->name << " " << name_variable.first
                   << ";" << std::endl;
        }

        for (auto name_type : this->types()) {
            name_type.second->write_header(stream, indent);
        }

        for (auto name_space : this->namespaces()) {
            name_space.second->write_header(stream, indent);
        }
    }

    const std::unordered_set<std::string> Project::sources() const {
        return this->_sources;
    }
}
