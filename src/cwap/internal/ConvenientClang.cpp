#include "cwap/internal/ConvenientClang.hpp"

#include <fstream>
#include <vector>

using std::ifstream;
using std::string;
using std::vector;

namespace cwap {
    namespace internal {
        string get_name(const CXCursor& cursor) {
            CXString spell = clang_getCursorSpelling(cursor);
            string name(clang_getCString(spell));
            clang_disposeString(spell);
            return name;
        }

        string get_type_name(const CXCursor& cursor) {
            CXType clang_type = clang_getCursorType(cursor);
            CXString type_spelling = clang_getTypeSpelling(clang_type);
            string type_name(clang_getCString(type_spelling));
            clang_disposeString(type_spelling);
            return type_name;
        }

        string get_usr(const CXCursor& cursor) {
            CXString spell = clang_getCursorUSR(cursor);
            string usr(clang_getCString(spell));
            clang_disposeString(spell);
            return usr;
        }

        string get_usr(const CXType& type) {
            return get_usr(clang_getTypeDeclaration(type));
        }

        string get_source_code(const CXCursor& cursor) {
            auto src_range = clang_getCursorExtent(cursor);
            auto loc_b = clang_getRangeStart(src_range);
            auto loc_e = clang_getRangeEnd(src_range);
            CXFile f = nullptr;
            unsigned line_s, col_s, offset_s;
            unsigned line_e, col_e, offset_e;
            clang_getExpansionLocation(loc_b, &f, &line_s, &col_s, &offset_s);
            clang_getExpansionLocation(loc_e, &f, &line_e, &col_e, &offset_e);

            CXString cx_file_name = clang_getFileName(f);
            ifstream source_file(clang_getCString(cx_file_name));
            clang_disposeString(cx_file_name);

            auto content_length = offset_e - offset_s;
            vector<char> buffer(content_length);
            source_file.seekg(offset_s);
            source_file.read(buffer.data(), content_length);

            string content(buffer.begin(), buffer.end());
            return content;
        }
    }
}
