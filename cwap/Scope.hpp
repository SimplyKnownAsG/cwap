#pragma once

#include "cwap/Function.hpp"
#include "cwap/Type.hpp"
#include "cwap/Variable.hpp"

#include <clang-c/Index.h>

#include <iostream>
#include <string>
#include <unordered_map>

namespace cwap {

    class Scope {
    private:
        std::unordered_map<std::string, Type*> _types;

        std::unordered_map<std::string, Variable*> _variables;

        std::unordered_map<std::string, Function*> _functions;

    protected:
        static CXChildVisitResult VisitChildrenCallback(CXCursor cursor,
                                                        CXCursor parent,
                                                        CXClientData client_data);

        Scope(const std::string name);

    public:
        const std::string name;

        Type* get_type(CXType& clang_type);

        const std::unordered_map<std::string, Type*> types() const;

        const std::unordered_map<std::string, Variable*> variables() const;

        const std::unordered_map<std::string, Function*> functions() const;
    };
}
