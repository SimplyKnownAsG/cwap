#pragma once

#include <ostream>

#include <clang-c/Index.h>

namespace cwap {
    enum class Access { PUBLIC, PRIVATE, PROTECTED, INTERNAL };

    Access get_access(CXCursor const& cursor);

    Access get_access(CX_CXXAccessSpecifier cxx_access);

    std::ostream& operator<<(std::ostream& stream, const Access& access);
}
