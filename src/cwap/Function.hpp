#pragma once

#include <ostream>
#include <string>
#include <vector>

namespace cwap {
    class TypeUsage;
    class Project;
    class Type;

    namespace internal {
        class Factory;
    }

    class Function {

        friend class internal::Factory;

    public:
        Function(const Type* return_type,
                 std::string name,
                 std::string usr,
                 std::string source_code = "")
          : return_type(return_type)
          , name(name)
          , usr(usr)
          , source_code(source_code){};

        const Type* return_type;

        const std::string name;

        const std::string usr;

        const std::string source_code;

    private:
        std::vector<TypeUsage*> _parameters;

    public:
        const std::vector<TypeUsage*> parameters() const {
            return this->_parameters;
        };

        void write_header(std::ostream& stream, std::string indent) const;

        friend std::ostream& operator<<(std::ostream& stream, const Function& self);
    };
}
