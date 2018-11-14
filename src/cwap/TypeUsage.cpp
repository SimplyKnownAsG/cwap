#include "cwap/TypeUsage.hpp"
#include "cwap/Project.hpp"
#include "cwap/Type.hpp"

#include <ostream>
#include <string>

namespace cwap {

    TypeUsage::TypeUsage(std::string usr, std::string name, Type* cwap_type)
      : usr(usr)
      , name(name)
      , cwap_type(cwap_type) {}

    TypeUsage* TypeUsage::Create(Project& project, const CXCursor& cursor) {
        auto type = project.get(clang_getCursorType(cursor));
        TypeUsage* variable = new TypeUsage(get_usr(cursor), get_name(cursor), type);
        return variable;
    }

    std::ostream& operator<<(std::ostream& stream, const TypeUsage& self) {
        stream << "<TypeUsage " << self.name << " " << *self.cwap_type << ">";
        return stream;
    }
}
