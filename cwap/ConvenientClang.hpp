#pragma once

#include <clang-c/Index.h>
#include <string>

namespace cwap {
    std::string get_name(const CXCursor& cursor);

    std::string get_usr(const CXCursor& cursor);

    class Function;
    class Namespace;
    class Type;

    class Clanger {
    public:
        static Function* create_function(const CXCursor& cursor, Namespace* space);
    };
}
