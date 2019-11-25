#include "cwap/interfaces/Interface.hpp"
#include <iostream>

namespace cwap {
    namespace interfaces {

        Interface::Interface(Type const& t)
          : _implemented(UNCHECKED)
          , _type(t) {}

        bool Interface::implemented() {
            if (this->_implemented == UNCHECKED) {
                this->_implemented = this->check_implementation() ? IMPLEMENTED : NOT_IMPLEMENTED;
            }
            return this->_implemented == IMPLEMENTED;
        }

    }
}
