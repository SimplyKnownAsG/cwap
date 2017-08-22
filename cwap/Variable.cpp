#include "cwap/Variable.hpp"
#include "cwap/ConvenientClang.hpp"
#include "cwap/Namespace.hpp"

namespace cwap {

    Variable::Variable(std::string name, std::string spacename, Type* cwap_type)
      : name(name)
      , spacename(spacename) {
        this->cwap_type = cwap_type;
    }

    Variable* Variable::Factory(CXCursor& cursor, Namespace* space) {
        CXType clang_type = clang_getCursorType(cursor);
        Type* type = space->get_type(clang_type);
        Variable* variable = new Variable(get_name(cursor), "", type);
        return variable;
    }

    std::ostream& operator<<(std::ostream& stream, const Variable& self) {
        stream << "<Variable " << self.name << " " << *(self.cwap_type) << ">";
        return stream;
    }
}
