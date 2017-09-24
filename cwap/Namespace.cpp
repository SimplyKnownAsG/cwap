#include "cwap/Namespace.hpp"
#include "cwap/ConvenientClang.hpp"
#include "cwap/Location.hpp"
#include "cwap/Project.hpp"

#include <string>

namespace cwap {

    Namespace::Namespace(std::string name)
      : name(name) {}

    Namespace::Namespace(std::string name, Project* project)
      : name(name) {
        this->project = project;
    }

    CXChildVisitResult Namespace::visit(CXCursor& cursor, CXCursor& parent) {
        Location location = Location::Create(cursor);
        if (location.file_name != this->project->filename) {
            return CXChildVisit_Continue;
        }
        CXCursorKind cursor_kind = clang_getCursorKind(cursor);
        if (!clang_isDeclaration(cursor_kind)) {
            return CXChildVisit_Continue;
        }
        switch (cursor.kind) {
        case CXCursor_VarDecl: {
            Variable* cv = Variable::Factory(cursor, this->get_type(cursor), this);
            this->_variables[cv->name] = cv;
            break;
        }
        case CXCursor_FunctionDecl: {
            Function* cf = Function::Factory(cursor, this);
            if (cf != NULL) {
                this->_functions.push_back(cf);
            }
            break;
        }
        case CXCursor_ClassDecl:
        case CXCursor_StructDecl: {
            Type* type = Type::Factory(cursor, this);
            this->_types[type->name] = type;
            // recursive
            clang_visitChildren(cursor, Namespace::VisitChildrenCallback, type);
            break;
        }
        case CXCursor_Namespace: {
            std::string space_name = get_name(cursor);
            Namespace* sub_space = NULL;
            if (this->_namespaces.count(space_name)) {
                sub_space = this->_namespaces.at(space_name);
            } else {
                sub_space = new Namespace(space_name, this->project);
                this->_namespaces[space_name] = sub_space;
            }
            // recursive!
            clang_visitChildren(cursor, Type::VisitChildrenCallback, sub_space);
            break;
        }
        default: {
            CXString cursor_kind_name = clang_getCursorKindSpelling(cursor.kind);
            std::cerr << "I do not know how to interpret declaration of "
                      << clang_getCString(cursor_kind_name) << " (" << cursor.kind << ")"
                      << std::endl;
            clang_disposeString(cursor_kind_name);
            break;
        }
        }

        return CXChildVisit_Continue;
    }

    Type* Namespace::get_type(CXCursor& cursor) {
        CXType clang_type = clang_getCursorType(cursor);
        return this->get_type(clang_type);
    }

    Type* Namespace::get_type(CXType& clang_type) {
        Type* ct = Type::Factory(clang_type, this);
        if (this->_types.count(ct->name)) {
            auto temp = ct;
            ct = this->_types[ct->name];
            delete temp;
        } else {
            this->_types[ct->name] = ct;
        }
        return ct;
    }

    const std::unordered_map<std::string, Type*> Namespace::types() const {
        return this->_types;
    }

    const std::unordered_map<std::string, Variable*> Namespace::variables() const {
        return this->_variables;
    }

    const std::vector<Function*> Namespace::functions() const {
        return this->_functions;
    }

    const std::unordered_map<std::string, Namespace*> Namespace::namespaces() const {
        return this->_namespaces;
    }

    void Namespace::dump_yaml(std::ostream& stream) {
        stream << "{" << std::endl;
        stream << "namespace: '" << this->name << "'," << std::endl;

        stream << "functions: [" << std::endl;
        for (auto func : this->functions()) {
            func->dump_yaml(stream);
            stream << "," << std::endl;
        }
        stream << "]," << std::endl;

        stream << "variables: [" << std::endl;
        for (auto name_variable : this->variables()) {
            name_variable.second->dump_yaml(stream);
            stream << "," << std::endl;
        }
        stream << "]," << std::endl;

        stream << "types: [" << std::endl;
        for (auto name_type : this->types()) {
            name_type.second->dump_yaml(stream);
            stream << "," << std::endl;
        }
        stream << "]," << std::endl;

        stream << "namespaces: [" << std::endl;
        for (auto name_space : this->namespaces()) {
            name_space.second->dump_yaml(stream);
            stream << "," << std::endl;
        }
        stream << "]," << std::endl;

        stream << "}" << std::endl;
    }
}
