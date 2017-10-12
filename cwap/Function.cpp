#include "cwap/Function.hpp"
#include "cwap/Namespace.hpp"
#include "cwap/Parameter.hpp"

namespace cwap {

    void Function::dump_yaml(std::ostream& stream) const {
        stream << "{" << std::endl;
        stream << "name: '" << this->name << "'," << std::endl;
        stream << "return_type: '" << this->return_type->name << "'," << std::endl;

        stream << "parameters: [" << std::endl;
        for (auto param : this->parameters()) {
            /* param->dump_yaml(stream); */
            stream << "," << std::endl;
        }
        stream << "}" << std::endl;
    }
}
