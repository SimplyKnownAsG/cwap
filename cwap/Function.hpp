#pragma once

#include "cwap/Type.hpp"
#include "cwap/Variable.hpp"

#include <clang-c/Index.h>

#include <string>
#include <vector>

namespace cwap {

    class Namespace;

    class Function {
    private:
        friend class Namespace;

        std::vector<Variable*> _arguments;

        Function(std::string name, Namespace const* space);

        static Function* Factory(CXCursor& cursor, Namespace* space);

    public:
        const std::string name;

        const Namespace* space;

        const Type* return_type;

        const std::vector<Variable*> arguments() const;
    };
}
