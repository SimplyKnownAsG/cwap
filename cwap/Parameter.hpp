#pragma once

#include "cwap/TypeUsage.hpp"

namespace cwap {

    class Function;

    class Parameter : public TypeUsage<Parameter, Function> {
        friend class Function;
        using TypeUsage<Parameter, Function>::TypeUsage;
    };

    extern template class TypeUsage<Parameter, Function>;
}
