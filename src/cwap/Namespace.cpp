#include "cwap/Namespace.hpp"
#include "cwap/Location.hpp"
#include "cwap/Project.hpp"

#include <algorithm>
#include <string>

namespace cwap {

    Namespace::Namespace(const std::string usr, const std::string name)
      : usr(usr)
      , name(name) {}

    const std::unordered_map<std::string, Type*> Namespace::types() const {
        return this->_types;
    }

    const std::unordered_map<std::string, TypeUsage*> Namespace::variables() const {
        return this->_variables;
    }

    std::vector<Function*> Namespace::functions() const {
        std::vector<Function*> funcs(this->_functions.begin(), this->_functions.end());
        return funcs;
    }

    const std::unordered_map<std::string, Namespace*> Namespace::namespaces() const {
        return this->_namespaces;
    }

    void Namespace::write_header(std::ostream& stream, std::string indent) const {
        stream << indent << "namespace " << this->name << " {" << std::endl;

        auto sub_indent = indent + "    ";
        for (auto func : this->functions()) {
            func->write_header(stream, sub_indent);
        }

        for (auto name_variable : this->variables()) {
            stream << indent << name_variable.second->cwap_type->name << " " << name_variable.first
                   << ";" << std::endl;
        }

        for (auto name_type : this->types()) {
            name_type.second->write_header(stream, sub_indent);
        }

        for (auto name_space : this->namespaces()) {
            name_space.second->write_header(stream, sub_indent);
        }

        stream << indent << "}" << std::endl;
    }

    std::ostream& operator<<(std::ostream& stream, const Namespace& self) {
        stream << "<Namespace " << self.name << ">";
        return stream;
    }
}
