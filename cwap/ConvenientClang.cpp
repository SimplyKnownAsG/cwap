#include "cwap/ConvenientClang.hpp"

namespace cwap {
    std::string get_name(const CXCursor& cursor) {
        CXString spell = clang_getCursorSpelling(cursor);
        std::string name(clang_getCString(spell));
        clang_disposeString(spell);
        return name;
    }
}
