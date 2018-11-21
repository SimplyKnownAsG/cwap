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

    std::ostream& operator<<(std::ostream& stream, const TypeUsage& self) {
        stream << "<TypeUsage " << self.name << " " << *self.cwap_type << ">";
        return stream;
    }
}
