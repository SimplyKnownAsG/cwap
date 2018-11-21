#pragma once

#include "cwap/Namespace.hpp"

#include <string>
#include <unordered_map>
#include <unordered_set>

using std::string;

namespace cwap {

    class Project : public Namespace {
    private:
        friend class ClangVisitor;

        std::unordered_set<string> _sources;

        friend CXChildVisitResult find_RENAME_THIS(CXCursor cursor,
                                                   CXCursor parent,
                                                   CXClientData client_data);

    public:
        Project(string name);

        ~Project() = default;

        std::unordered_map<string, string> type_renames;

        void process_options(string filename);

        const std::unordered_set<string> sources() const;

        void parse(string filename);

        void parse(string filename, std::vector<string> clang_args);

        void parse(std::vector<string> filenames, std::vector<string> clang_args);

        void write_yaml();

        void write_header(std::ostream& stream, std::string indent = "") const override;

        friend std::ostream& operator<<(std::ostream& stream, const Project& self) {
            stream << "<Project " << self.name << ">";
            return stream;
        };
    };
}
