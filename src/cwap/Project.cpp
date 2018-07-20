#include "cwap/Project.hpp"
#include "cwap/Location.hpp"

#include "yaml-cpp/yaml.h"

#include <clang-c/Index.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace cwap {

    Project::Project(std::string name)
      : Namespace("", name) {
        this->process_options("/Users/graham/Projects/cwap/conf/standard.yaml");
    }

    struct RenameThisData {
        Project& project;
        const std::string& new_name;
    };

    CXChildVisitResult find_RENAME_THIS(CXCursor cursor,
                                        CXCursor parent,
                                        CXClientData client_data) {
        Location location = Location::Create(cursor);
        if (location.file_name != "cwap-temp-source.hpp") {
            return CXChildVisit_Continue;
        }

        CXCursorKind cursor_kind = clang_getCursorKind(cursor);
        if (!clang_isDeclaration(cursor_kind)) {
            return CXChildVisit_Continue;
        }

        std::string name = get_name(cursor);

        if (name == "RENAME_THIS") {
            Project& project = ((struct RenameThisData*)client_data)->project;
            const std::string& new_name = ((struct RenameThisData*)client_data)->new_name;
            std::string type_name = get_type_name(cursor);
            project.type_renames[type_name] = new_name;
            return CXChildVisit_Break;
        }

        return CXChildVisit_Continue;
    }

    void Project::process_options(std::string filename) {
        std::cout << "loading " << filename << std::endl;
        YAML::Node config = YAML::LoadFile(filename);
        YAML::Node type_renames = config["type renames"];
        if (type_renames) {
            for (YAML::const_iterator it = type_renames.begin(); it != type_renames.end(); ++it) {
                std::string new_name = it->first.as<std::string>();
                std::string text = it->second.as<std::string>();
                std::ofstream temp_file("cwap-temp-source.hpp");
                temp_file << text;
                temp_file.close();
                const char* args[0];

                CXIndex index = clang_createIndex(1, 1);
                CXTranslationUnit tu =
                        clang_parseTranslationUnit(index,
                                                   "cwap-temp-source.hpp",
                                                   args,
                                                   0,
                                                   NULL,
                                                   0,
                                                   CXTranslationUnit_SkipFunctionBodies |
                                                           CXTranslationUnit_PrecompiledPreamble |
                                                           CXTranslationUnit_Incomplete);
                std::remove("cwap-temp-source.hpp");

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
                    CXTranslationUnit_SkipFunctionBodies | CXTranslationUnit_PrecompiledPreamble |
                            CXTranslationUnit_Incomplete);

            if (tu == NULL) {
                clang_disposeTranslationUnit(tu);
                throw std::invalid_argument("Cannot create translation unit");
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
