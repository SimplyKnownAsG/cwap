#pragma once

#include "cwap/CwapType.hpp"

#include <string>

namespace cwap {

    class CwapVariable {
    public:
        const std::string name;

        const std::string spacename;

        CwapType* cwap_type;

    protected:
        friend class CwapNamespace;

        CwapVariable(std::string name, std::string spacename, CwapType* cwap_type);
    };
}
