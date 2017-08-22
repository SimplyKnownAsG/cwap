#pragma once

#include "cwap/TypeUsage.hpp"

namespace cwap {

    class Variable : public TypeUsage<Variable, Namespace> {
        friend class Namespace;
        using TypeUsage<Variable, Namespace>::TypeUsage;
    };

    extern template class TypeUsage<Variable, Namespace>;
}
