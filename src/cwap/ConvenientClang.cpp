#include "cwap/ConvenientClang.hpp"

namespace cwap {
    std::string get_name(const CXCursor& cursor) {
        CXString spell = clang_getCursorSpelling(cursor);
        std::string name(clang_getCString(spell));
        clang_disposeString(spell);
        return name;
    }

    std::string get_type_name(const CXCursor& cursor) {
        CXType clang_type = clang_getCursorType(cursor);
        CXString type_spelling = clang_getTypeSpelling(clang_type);
        std::string type_name(clang_getCString(type_spelling));
        clang_disposeString(type_spelling);
        return type_name;
    }

    std::string get_usr(const CXCursor& cursor) {
        CXString spell = clang_getCursorUSR(cursor);
        std::string usr(clang_getCString(spell));
        clang_disposeString(spell);
        return usr;
    }

    std::string get_usr(const CXType& type) {
        return get_usr(clang_getTypeDeclaration(type));
    }
}
