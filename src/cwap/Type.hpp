#pragma once

#include "cwap/ClangVisitor.hpp"
#include "cwap/Function.hpp"

#include <clang-c/Index.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace cwap {

    class Project;
    class Type;
    class Attribute;

    class Type : protected ClangVisitor {
    private:
        std::unordered_map<std::string, Type*> _types;

        std::unordered_map<std::string, Attribute*> _attributes;

        std::vector<Function*> _methods;

    public:
        virtual ~Type() = default;

        const std::string name;

        const bool is_basic;

        const bool is_struct;

        const bool is_class;

        const std::unordered_map<std::string, Type*> types() const;

        const std::unordered_map<std::string, Attribute*> attributes() const;

        const std::vector<Function*> methods() const;

        void dump_yaml(std::ostream& stream) const;

        bool has_function(std::string usr) const;

        const std::string get_namespace_name() const;

    private:
        friend class Namespace;

        Type(std::string name, bool is_basic, bool is_struct, bool is_class);

        static Type* Factory(CXType& cursor);

        static Type* Factory(CXCursor& cursor);

        CXChildVisitResult visit(CXCursor& cursor, Project& project) override;

        friend std::ostream& operator<<(std::ostream& stream, const Type& self);
    };
}
