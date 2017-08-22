#pragma once

#include "cwap/TypeUsage.hpp"

namespace cwap {

    class Attribute : public TypeUsage<Attribute, Type> {
        friend class Type;
        using TypeUsage<Attribute, Type>::TypeUsage;
    };

    extern template class TypeUsage<Attribute, Type>;
}
