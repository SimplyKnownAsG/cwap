#include "cwap/Type.hpp"
#include "cwap/Namespace.hpp"

namespace cwap {

    Type::Type(std::string name,
               Namespace const* space,
               bool is_basic,
               bool is_function,
               bool is_struct,
               bool is_class,
               bool is_static)
      : name(name)
      , space(space)
      , is_basic(is_basic)
      , is_function(is_function)
      , is_struct(is_struct)
      , is_class(is_class)
      , is_static(is_static){};

    Type* Type::Factory(CXType& type, Namespace const* space) {
        CXString type_spelling = clang_getTypeSpelling(type);
        std::string type_name(clang_getCString(type_spelling));
        clang_disposeString(type_spelling);
        return new Type(type_name,
                        space,
                        CXType_Void < type.kind && type.kind < CXType_NullPtr,
                        false,
                        false,
                        false,
                        false);
    }

    std::ostream& operator<<(std::ostream& stream, const Type& self) {
        stream << "<Type ";

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
