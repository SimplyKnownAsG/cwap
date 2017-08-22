#pragma once

#include "cwap/Parameter.hpp"
#include "cwap/Type.hpp"

#include <clang-c/Index.h>

#include <string>
#include <vector>

namespace cwap {

    class Namespace;

    class Function {
    private:
        friend class Namespace;
        friend class Type;

        std::vector<Parameter*> _parameters;

        Function(std::string name, const Namespace* space, const Type* bound_parent);

        static Function* Factory(CXCursor& cursor, Namespace* space, Type* parent);

        static Function* Factory(CXCursor& cursor, Namespace* space);

    public:
        const std::string name;

        const Namespace* space;

        const Type* bound_parent;

        const Type* return_type;

        const std::vector<Parameter*> parameters() const;
    };
}
