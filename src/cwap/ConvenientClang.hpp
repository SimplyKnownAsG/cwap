#pragma once

#include <clang-c/Index.h>
#include <string>

namespace cwap {
    std::string get_name(const CXCursor& cursor);

    std::string get_type_name(const CXCursor& cursor);

    std::string get_usr(const CXCursor& cursor);

    std::string get_usr(const CXType& type);
}
