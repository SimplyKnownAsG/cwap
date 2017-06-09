#pragma once

#include "cwap/CwapType.hpp"
#include "cwap/CwapVariable.hpp"

#include <clang-c/Index.h>

#include <string>
#include <vector>

namespace cwap {

    class CwapNamespace;

    class CwapFunction {
    private:
        friend class CwapNamespace;

        std::vector<CwapVariable*> _arguments;

        CwapFunction(std::string name,
                     CwapNamespace const* spacename,
                     bool is_basic,
                     bool is_struct,
                     bool is_function,
                     bool is_class,
                     bool is_static);

    public:
        const std::string name;

        const CwapNamespace* spacename;

        const CwapType* return_type;

        const std::vector<CwapVariable*> arguments() const;
    };
}
