#include "cwap/ConvenientClang.hpp"

namespace cwap {
    std::string get_name(const CXCursor& cursor) {
        CXString spell = clang_getCursorSpelling(cursor);
        std::string name(clang_getCString(spell));
        clang_disposeString(spell);
        return name;
    }

    std::string get_usr(const CXCursor& cursor) {
        CXString spell = clang_getCursorUSR(cursor);
        std::string usr(clang_getCString(spell));
        clang_disposeString(spell);
        return usr;
    }
}
