#include "cwap/Function.hpp"
#include "cwap/Namespace.hpp"
#include "cwap/Parameter.hpp"

namespace cwap {

    void Function::dump_yaml(std::ostream& stream) const {
        stream << "{" << std::endl;
        stream << "name: '" << this->name << "'," << std::endl;
        stream << "return_type: '" << this->return_type->name << "'," << std::endl;

        stream << "parameters: [" << std::endl;
        for (auto param = this->parameters().begin(), e = this->parameters().end(); param != e;
             param++) {
            (*param)->dump_yaml(stream);
            if (param != e) {
                stream << "," << std::endl;
            }
        }
        stream << "]" << std::endl;
    }
}
