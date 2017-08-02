#include "cwap/CwapType.hpp"
#include "cwap/CwapNamespace.hpp"

namespace cwap {

    CwapType::CwapType(std::string name, CwapNamespace const* spacename, bool is_basic,
                       bool is_function, bool is_struct, bool is_class, bool is_static)
      : name(name)
      , spacename(spacename)
      , is_basic(is_basic)
      , is_function(is_function)
      , is_struct(is_struct)
      , is_class(is_class)
      , is_static(is_static){};

    CwapType* CwapType::Factory(CXCursor& cursor, CwapNamespace const* spacename) {
        CXType type = clang_getCursorType(cursor);
        CXString type_spelling = clang_getTypeSpelling(type);
        std::string type_name(clang_getCString(type_spelling));
        clang_disposeString(type_spelling);
        return new CwapType(type_name, spacename,
                            CXType_Void < type.kind && type.kind < CXType_NullPtr, false, false,
                            false, false);
    }

    std::ostream& operator<<(std::ostream& stream, const CwapType& self) {
        stream << "<CwapType ";

        if (self.is_static) {
            stream << "static ";
        }
        if (self.is_basic) {
            stream << "basic type ";
        }
        if (self.is_function) {
            stream << "function ";
        }
        if (self.is_struct) {
            stream << "struct ";
        }
        if (self.is_class) {
            stream << "class ";
        }
        stream << self.name << ">";
        return stream;
    }
}
