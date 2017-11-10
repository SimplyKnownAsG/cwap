#include "cwap/Namespace.hpp"
#include "cwap/ConvenientClang.hpp"
#include "cwap/Location.hpp"
#include "cwap/Project.hpp"

#include <string>

namespace cwap {

    Namespace::Namespace(std::string name)
      : name(name) {}

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
            Variable* cv = Variable::Factory(cursor, project.get_type(cursor));
            this->_variables[cv->name] = cv;
            break;
        }
        case CXCursor_FunctionDecl: {
            Function* cf = Clanger::create_function(cursor, project);
            if (cf != NULL) {
                this->_functions.push_back(cf);
            }
            break;
        }
        case CXCursor_ClassDecl:
        case CXCursor_StructDecl: {
            Type* type = Type::Factory(cursor);
            this->_types[type->name] = type;
            // recursive
            struct ClangVisitorData type_wrapper {
                type, project
            };
            clang_visitChildren(cursor, Namespace::VisitChildrenCallback, &type_wrapper);
            break;
        }
        case CXCursor_Namespace: {
            std::string space_name = get_name(cursor);
            Namespace* sub_space = NULL;
            if (this->_namespaces.count(space_name)) {
                sub_space = this->_namespaces.at(space_name);
            } else {
                sub_space = new Namespace(space_name);
                this->_namespaces[space_name] = sub_space;
            }
            // recursive!
            struct ClangVisitorData type_wrapper {
                sub_space, project
            };
            clang_visitChildren(cursor, Type::VisitChildrenCallback, &type_wrapper);
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

    // TODO: this should only ever be called on a Project instance
    Type* Namespace::get_type(CXCursor& cursor) {
        CXType clang_type = clang_getCursorType(cursor);
        return this->get_type(clang_type);
    }

    // TODO: this should only ever be called on a Project instance
    Type* Namespace::get_type(CXType& clang_type) {
        Type* ct = Type::Factory(clang_type);
        Namespace* space = this->get_namespace(ct->get_namespace_name());
        if (space->_types.count(ct->name)) {
            auto temp = ct;
            ct = space->_types[ct->name];
            delete temp;
        } else {
            space->_types[ct->name] = ct;
        }
        return ct;
    }

    // TODO: this can be made faster where a::b::c can be short circuited
    Namespace* Namespace::get_namespace(std::string name) {
        if (name == "")
            return this;
        for (auto name_space : this->_namespaces) {
            if (name_space.first == name) {
                return name_space.second;
            }
            if (Namespace* child_space = name_space.second->get_namespace(name)) {
                return child_space;
            }
        }
        Namespace* result = this->_namespaces[name] = new Namespace(name);
        return result;
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

    bool Namespace::has_function(std::string usr) const {
        for (auto func : this->_functions) {
            if (func->usr == usr) {
                return true;
            }
        }
        for (auto name_space : this->_namespaces) {
            if (name_space.second->has_function(usr)) {
                return true;
            }
        }
        for (auto name_type : this->_types) {
            if (name_type.second->has_function(usr)) {
                return true;
            }
        }
        return false;
    }
}
