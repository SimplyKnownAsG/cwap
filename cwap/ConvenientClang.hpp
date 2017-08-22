#pragma once

#include <clang-c/Index.h>
#include <string>

namespace cwap {
    std::string get_name(const CXCursor& cursor);
}
