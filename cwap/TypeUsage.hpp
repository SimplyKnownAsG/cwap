#pragma once

#include "cwap/ConvenientClang.hpp"
#include "cwap/Type.hpp"

#include <ostream>
#include <string>

namespace cwap {

    template<class TConcrete>
    class TypeUsage {
    public:
        const std::string name;

        Type* cwap_type;

        void dump_yaml(std::ostream& stream) const {
            stream << "{" << std::endl;
            stream << "name: '" << this->name << "'," << std::endl;
            stream << "type: '" << this->cwap_type->name << "'" << std::endl;
            stream << "}" << std::endl;
        }

    protected:
        static TConcrete* Factory(const CXCursor& cursor, Type* type) {
            TConcrete* variable = new TConcrete(get_name(cursor), type);
            return variable;
        };

        TypeUsage(std::string name, Type* cwap_type)
          : name(name)
          , cwap_type(cwap_type){};
    };
}
