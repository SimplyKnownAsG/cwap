#include "cwap/Function.hpp"
#include "cwap/Namespace.hpp"
#include "cwap/Project.hpp"
#include "cwap/TypeUsage.hpp"

namespace cwap {

    void Function::write_header(std::ostream& stream, std::string indent) const {
        stream << indent << this->return_type->name << " " << this->name << "(";

        for (auto param = this->parameters().begin(), e = this->parameters().end(); param != e;
             param++) {
            auto p = *param;
            stream << p->cwap_type->name;
            stream << " " << p->name;

            if (param != e) {
                stream << "," << std::endl;
            }
        }

        stream << ");" << std::endl;
    }

    std::ostream& operator<<(std::ostream& stream, const Function& self) {
        stream << "<Function ";

        stream << *(self.return_type);
        stream << self.name;
        stream << "(";

        for (auto param : self._parameters) {
            stream << *param;
            stream << ", ";
        }
        stream << self.name << ")>";
        return stream;
    }
}
