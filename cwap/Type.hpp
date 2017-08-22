#pragma once

#include <clang-c/Index.h>

#include <string>

namespace cwap {

    class Namespace;

    class Type {
    protected:
        friend class Namespace;

        Type(std::string name,
             Namespace const* space,
             bool is_basic,
             bool is_struct,
             bool is_class,
             bool is_static);

        static Type* Factory(CXType& cursor, Namespace const* space);

    public:
        const std::string name;

        Namespace const* space;

        const bool is_basic;

        const bool is_struct;

        const bool is_class;

        const bool is_static;

        friend std::ostream& operator<<(std::ostream& stream, const Type& self);
    };
}
