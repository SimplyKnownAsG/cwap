#include "cwap/CwapVariable.hpp"

namespace cwap {

    CwapVariable::CwapVariable(std::string name, std::string spacename, CwapType* cwap_type)
      : name(name)
      , spacename(spacename) {
        this->cwap_type = cwap_type;
    }
}
