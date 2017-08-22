#pragma once

#include "cwap/ClangVisitor.hpp"
#include "cwap/Function.hpp"
#include "cwap/Type.hpp"
#include "cwap/Variable.hpp"

#include <clang-c/Index.h>

#include <iostream>
#include <string>
#include <unordered_map>

namespace cwap {

    class Namespace;

    class Namespace : protected ClangVisitor {
    private:
        std::unordered_map<std::string, Type*> _types;

        std::unordered_map<std::string, Variable*> _variables;

        std::unordered_map<std::string, Function*> _functions;

        std::unordered_map<std::string, Namespace*> _namespaces;

    protected:
        CXChildVisitResult visit(CXCursor& cursor, CXCursor& parent) override;

        Namespace(const std::string name);

    public:
        const std::string name;

        Type* get_type(CXCursor& clang_type);

        Type* get_type(CXType& clang_type);

        const std::unordered_map<std::string, Type*> types() const;

        const std::unordered_map<std::string, Variable*> variables() const;

        const std::unordered_map<std::string, Function*> functions() const;

        const std::unordered_map<std::string, Namespace*> namespaces() const;
    };
}
