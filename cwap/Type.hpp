#pragma once

#include "cwap/ClangVisitor.hpp"

#include <clang-c/Index.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace cwap {

    class Namespace;
    class Type;
    class Attribute;
    class Function;

    class Type : protected ClangVisitor {
    private:
        std::unordered_map<std::string, Type*> _types;

        std::unordered_map<std::string, Attribute*> _attributes;

        std::vector<Function*> _methods;

    public:
        virtual ~Type() = default;

        const std::string name;

        Namespace const* space;

        Type const* parent;

        const bool is_basic;

        const bool is_struct;

        const bool is_class;

        const std::unordered_map<std::string, Type*> types() const;

        const std::unordered_map<std::string, Attribute*> attributes() const;

        const std::vector<Function*> methods() const;

        void dump_yaml(std::ostream& stream) const;

    private:
        friend class Namespace;

        Type(std::string name,
             Namespace const* space,
             bool is_basic,
             bool is_struct,
             bool is_class);

        static Type* Factory(CXType& cursor, Namespace const* space);

        static Type* Factory(CXCursor& cursor, Namespace const* space);

        CXChildVisitResult visit(CXCursor& cursor, CXCursor& parent) override;

        friend std::ostream& operator<<(std::ostream& stream, const Type& self);
    };
}
