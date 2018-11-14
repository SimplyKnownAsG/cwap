#pragma once

#include <ostream>
#include <string>

#include <clang-c/Index.h>

namespace cwap {
    class Project;
    class Type;

    class TypeUsage {
    public:
        const std::string usr;

        const std::string name;

        Type* cwap_type;

        static TypeUsage* Create(Project& project, const CXCursor& cursor);

    private:
        TypeUsage(std::string usr, std::string name, Type* cwap_type);

        friend std::ostream& operator<<(std::ostream& stream, const TypeUsage& self);
    };
}
