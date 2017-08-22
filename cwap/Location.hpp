#pragma once

#include <clang-c/Index.h>

#include <ostream>
#include <string>

namespace cwap {

    class Location {
    public:
        const std::string file_name;

        const unsigned line;

        const unsigned column;

        const unsigned offset;

    private:
        Location(std::string file_name, unsigned line, unsigned column, unsigned offset);

        friend class Namespace;

        static Location Create(const CXCursor& cursor);

        friend std::ostream& operator<<(std::ostream& stream, const Location& self);
    };
}
