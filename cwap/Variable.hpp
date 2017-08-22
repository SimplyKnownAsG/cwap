#pragma once

#include "cwap/Type.hpp"

#include <iostream>
#include <string>

namespace cwap {

    class Variable {
    public:
        const std::string name;

        const std::string spacename;

        Type* cwap_type;

    private:
        static Variable* Factory(CXCursor& cursor, Namespace* space);

    protected:
        friend class Namespace;
        friend class Function;

        Variable(std::string name, std::string spacename, Type* cwap_type);

        friend std::ostream& operator<<(std::ostream& stream, const Variable& self);
    };
}
