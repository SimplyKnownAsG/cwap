#include "cwap/CwapVariable.hpp"

namespace cwap {

    CwapVariable::CwapVariable(std::string name, std::string spacename, CwapType* cwap_type)
      : name(name)
      , spacename(spacename) {
        this->cwap_type = cwap_type;
    }

    std::ostream& operator<<(std::ostream& stream, const CwapVariable& self) {
        stream << "<CwapVariable " << self.name << " " << *(self.cwap_type) << ">";
        return stream;
    }
}
