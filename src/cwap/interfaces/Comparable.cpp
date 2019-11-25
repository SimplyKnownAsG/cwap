#include "cwap/interfaces/Comparable.hpp"
#include "cwap/TypeUsage.hpp"

namespace cwap {
    namespace interfaces {

        Comparable::Comparable(Type const& t)
          : Interface(t) {}

        bool Comparable::check_implementation() {
            for (auto method_name : { "operator<", "operator>", "operator==" }) {
                // XXX: naive implemenation, should use a map or something
                bool success = false;

                for (auto f : this->_type.methods()) {
                    if (f->name == method_name) {
                        if (f->return_type->name != "bool") {
                            return false;
                        }

                        auto rhs = f->parameters().at(0);

                        if (&this->_type == rhs->cwap_type) {
                            success = true;
                            break;
                        }
                    } else {
                    }
                }

                if (!success) {
                    return false;
                }
            }

            return true;
        }
    }
}
