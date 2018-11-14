#include "cwap/Namespace.hpp"
#include "cwap/ConvenientClang.hpp"
#include "cwap/Location.hpp"
#include "cwap/Project.hpp"

#include <algorithm>
#include <string>

namespace cwap {

    Namespace::Namespace(const std::string usr, const std::string name)
      : usr(usr)
      , name(name) {}

    Namespace* Namespace::Create(Project& project, const CXCursor& cursor) {
        if (cursor.kind != CXCursor_Namespace) {
            throw std::invalid_argument("Cursor is not CXCursor_Namespace");
        }

        auto result = new Namespace(get_usr(cursor), get_name(cursor));

        const CXCursor& parent = clang_getCursorSemanticParent(cursor);

        // TODO: could hide __cxx11 namespaces here
        if (parent.kind == CXCursor_Namespace) {
            // getting recursive
            auto space = project.get<Namespace>(parent);
            space->_namespaces[result->name] = result;
        } else {
            project._namespaces[result->name] = result;
        }

        return result;
    }

    CXChildVisitResult Namespace::visit(CXCursor& cursor, Project& project) {
        Location location = Location::Create(cursor);

        if (!project.sources().count(location.file_name)) {
            return CXChildVisit_Continue;
        }

        CXCursorKind cursor_kind = clang_getCursorKind(cursor);

        if (!clang_isDeclaration(cursor_kind)) {
            return CXChildVisit_Continue;
        }

        switch (cursor.kind) {
        case CXCursor_VarDecl: {
            auto variable = project.get<TypeUsage>(cursor);
            this->_variables[variable->name] = variable;
            break;
        }
        case CXCursor_FunctionDecl: {
            Function* cf = project.get<Function>(cursor);
            this->_functions.insert(cf);
            break;
        }
        case CXCursor_ClassDecl:
        case CXCursor_StructDecl: {
            Type* type = project.get(clang_getCursorType(cursor));
            this->_types[type->name] = type;
            // recursive
            struct ClangVisitorData type_wrapper {
                type, project
            };
            clang_visitChildren(cursor, Type::VisitChildrenCallback, &type_wrapper);
            break;
        }
        case CXCursor_Namespace: {
            Namespace* sub_space = project.get<Namespace>(cursor);
            this->_namespaces[sub_space->name] = sub_space;

            // recursive!
            struct ClangVisitorData type_wrapper {
                sub_space, project
            };

            clang_visitChildren(cursor, Namespace::VisitChildrenCallback, &type_wrapper);
            break;
        }
        default: {
            /* CXString cursor_kind_name = clang_getCursorKindSpelling(cursor.kind); */
            /* std::cerr << "I do not know how to interpret declaration of " */
            /*           << clang_getCString(cursor_kind_name) << " (" << cursor.kind << ")" */
            /*           << std::endl; */
            /* clang_disposeString(cursor_kind_name); */
            break;
        }
        }

        return CXChildVisit_Continue;
    }

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
