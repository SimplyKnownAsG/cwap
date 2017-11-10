#pragma once

#include "cwap/TypeUsage.hpp"

namespace cwap {

    class Variable : public TypeUsage<Variable> {
        friend class Namespace;
        using TypeUsage<Variable>::TypeUsage;
    };

    extern template class TypeUsage<Variable>;
}
