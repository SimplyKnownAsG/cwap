#pragma once

#include "cwap/Access.hpp"

#include <ostream>
#include <string>

#include <clang-c/Index.h>

namespace cwap {
    class Project;
    class Type;

    class TypeUsage {
    public:
        std::string const usr;

        std::string const name;

        Access const access;

        int const size;

        Type* cwap_type;

        static TypeUsage* Create(Project& project, const CXCursor& cursor);

    private:
        TypeUsage(std::string usr, std::string name, Access access, int size, Type* cwap_type);
    };

    std::ostream& operator<<(std::ostream& stream, const TypeUsage& self);
}
