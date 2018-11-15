#pragma once

#include "clang-c/Index.h"

#include <ostream>

namespace cwap {
    enum class Access { PUBLIC, PRIVATE, PROTECTED, INTERNAL };

    Access from_clang(CXCursor const& cursor);

    Access from_clang(CX_CXXAccessSpecifier cxx_access);

    std::ostream& operator<<(std::ostream& stream, const Access& access);
}
