#pragma once

#include "cwap/ConvenientClang.hpp"
#include "cwap/Type.hpp"

#include <string>

namespace cwap {

    template<class TConcrete, class TParent>
    class TypeUsage {
    public:
        const std::string name;

        Type* cwap_type;

        const TParent* parent;

    protected:
        static TConcrete* Factory(CXCursor& cursor, Type* type, TParent* parent) {
            TConcrete* variable = new TConcrete(get_name(cursor), type, parent);
            return variable;
        };

        TypeUsage(std::string name, Type* cwap_type, TParent* parent)
          : name(name)
          , cwap_type(cwap_type)
          , parent(parent){};
    };
}
