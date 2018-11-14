#pragma once

#include <ostream>
#include <string>
#include <vector>

#include <clang-c/Index.h>

namespace cwap {
    class TypeUsage;
    class Project;
    class Type;

    class Function {

    public:
        Function(const Type* return_type, std::string name, std::string usr)
          : return_type(return_type)
          , name(name)
          , usr(usr){};

        static Function* Create(Project& project, const CXCursor& cursor);

        const Type* return_type;

        const std::string name;

        const std::string usr;

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
