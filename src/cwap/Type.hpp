#pragma once

#include "cwap/ClangVisitor.hpp"
#include "cwap/Function.hpp"

#include <clang-c/Index.h>

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using std::string;

namespace cwap {

    class Project;
    class Type;
    class TypeUsage;

    class Type : protected ClangVisitor {
    private:
        std::unordered_map<string, Type*> _types;

        std::unordered_map<string, TypeUsage*> _attributes;

        std::unordered_set<Function*> _methods;

    public:
        virtual ~Type() = default;

        const string usr;

        const string name;

        const bool is_basic;

        const bool is_struct;

        const bool is_class;

        const std::unordered_map<string, Type*> types() const;

        const std::unordered_map<string, TypeUsage*> attributes() const;

        std::vector<Function*> methods() const;

        void write_header(std::ostream& stream, std::string indent) const;

    private:
        friend class Project;
        friend class Namespace;

        Type(const string usr, const string name, bool is_basic, bool is_struct, bool is_class);

        /* static Type* Create(Project& project, const CXCursor& cursor); */

        static Type* Create(Project& project, const CXType& clang_type);

        CXChildVisitResult visit(CXCursor& cursor, Project& project) override;

        friend std::ostream& operator<<(std::ostream& stream, const Type& self);
    };
}
