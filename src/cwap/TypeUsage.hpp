#pragma once

#include "cwap/Access.hpp"

#include <ostream>
#include <string>

namespace cwap {
    class Project;
    class Type;

    namespace internal {
        class Factory;
    }

    class TypeUsage {
        friend class internal::Factory;

    public:
        std::string const usr;

        std::string const name;

        Access const access;

        int const size;

        Type* cwap_type;

    private:
        TypeUsage(std::string usr, std::string name, Access access, int size, Type* cwap_type);
    };

    std::ostream& operator<<(std::ostream& stream, const TypeUsage& self);
}
