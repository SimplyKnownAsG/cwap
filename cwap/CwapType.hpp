#pragma once

#include <clang-c/Index.h>

#include <string>

namespace cwap {

    class CwapNamespace;

    class CwapType {
    protected:
        friend class CwapNamespace;

        CwapType(std::string name,
                 CwapNamespace const* spacename,
                 bool is_basic,
                 bool is_struct,
                 bool is_function,
                 bool is_class,
                 bool is_static);

        static CwapType* Factory(CXCursor& cursor, CwapNamespace const* spacename);

    public:
        const std::string name;

        CwapNamespace const* spacename;

        const bool is_basic;

        const bool is_function;

        const bool is_struct;

        const bool is_class;

        const bool is_static;
    };
}
