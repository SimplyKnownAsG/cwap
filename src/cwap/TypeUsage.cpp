#include "cwap/TypeUsage.hpp"
#include "cwap/Project.hpp"
#include "cwap/Type.hpp"

#include <ostream>
#include <string>

namespace cwap {

    TypeUsage::TypeUsage(std::string usr,
                         std::string name,
                         Access access,
                         int size,
                         Type* cwap_type)
      : usr(usr)
      , name(name)
      , access(access)
      , size(size)
      , cwap_type(cwap_type) {}

    TypeUsage* TypeUsage::Create(Project& project, const CXCursor& cursor) {
        auto clang_access = clang_getCXXAccessSpecifier(cursor);
        auto clang_type = clang_getCursorType(cursor);
        auto type = project.get(clang_type);
        int size = clang_Type_getSizeOf(clang_type);
        TypeUsage* variable =
                new TypeUsage(get_usr(cursor), get_name(cursor), from_clang(cursor), size, type);
        return variable;
    }

    std::ostream& operator<<(std::ostream& stream, const TypeUsage& self) {
        stream << "<TypeUsage " << self.name << " " << *self.cwap_type << ">";
        return stream;
    }
}
