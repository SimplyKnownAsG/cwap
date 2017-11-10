#pragma once

#include "cwap/TypeUsage.hpp"

namespace cwap {

    class Attribute : public TypeUsage<Attribute> {
        friend class Type;
        using TypeUsage<Attribute>::TypeUsage;
    };

    // force it to be compiled in cpp file
    extern template class TypeUsage<Attribute>;
}
