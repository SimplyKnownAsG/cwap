#include "cwap/Location.hpp"

namespace cwap {

    /* class Project; */

    Location::Location(std::string file_name, unsigned line, unsigned column, unsigned offset)
      : file_name(file_name)
      , line(line)
      , column(column)
      , offset(offset){};

    Location Location::Create(const CXCursor& cursor) {
        CXSourceLocation loc = clang_getCursorLocation(cursor);
        CXFile file;
        unsigned line, column, offset;
        clang_getSpellingLocation(loc, &file, &line, &column, &offset);
        CXString cxfile_name = clang_getFileName(file);
        std::string file_name(clang_getCString(cxfile_name));
        clang_disposeString(cxfile_name);
        return Location(file_name, line, column, offset);
    }

    std::ostream& operator<<(std::ostream& stream, const Location& self) {
        stream << self.file_name << ":" << self.line << ":" << self.column << ":" << self.offset;
        return stream;
    }
}
