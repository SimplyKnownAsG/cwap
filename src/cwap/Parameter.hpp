#pragma once

#include "cwap/TypeUsage.hpp"

namespace cwap {

    class Function;

    class Parameter : public TypeUsage<Parameter> {
        friend class Clanger;
        using TypeUsage<Parameter>::TypeUsage;
    };

    extern template class TypeUsage<Parameter>;
}
