#pragma once

#include <ostream>
#include <string>
#include <vector>

namespace cwap {
    class Parameter;
    class Type;

    class Function {

    public:
        Function(const Type* return_type, std::string name, std::string usr)
          : return_type(return_type)
          , name(name)
          , usr(usr){};

        const Type* return_type;

        const std::string name;

        const std::string usr;

    private:
        friend class Clanger;
        std::vector<Parameter*> _parameters;

    public:
        const std::vector<Parameter*> parameters() const {
            return this->_parameters;
        };

        void dump_yaml(std::ostream& stream) const;
    };
}
