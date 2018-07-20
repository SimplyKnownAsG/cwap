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

        return new Namespace(get_usr(cursor), get_name(cursor));
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

    /* Function* Namespace::get_function(std::string usr) const { */
    /*     for (auto func : this->_functions) { */
    /*         if (func->usr == usr) { */
    /*             return func; */
    /*         } */
    /*     } */
    /*     for (auto name_space : this->_namespaces) { */
    /*         if (Function* func = name_space.second->get_function(usr)) { */
    /*             return func; */
    /*         } */
    /*     } */
    /*     for (auto name_type : this->_types) { */
    /*         if (Function* func = name_type.second->get_function(usr)) { */
    /*             return func; */
    /*         } */
    /*     } */
    /*     return NULL; */
    /* } */

    /* // TODO: this can be made faster where a::b::c can be short circuited */
    /* Namespace* Namespace::get_namespace(std::string usr) const { */
    /*     for (auto name_space : this->_namespaces) { */
    /*         if (Namespace* space = name_space.second->get_namespace(usr)) { */
    /*             return space; */
    /*         } */
    /*     } */

    /*     return NULL; */
    /* } */

    /* Type* Namespace::get_type(std::string usr) const { */
    /*     for (auto type_name : this->_types) { */
    /*         if (type_name.second->usr == usr) { */
    /*             return type_name.second; */
    /*         } */
    /*     } */

    /*     for (auto name_space : this->_namespaces) { */
    /*         if (Type* type = name_space.second->get_type(usr)) { */
    /*             return type; */
    /*         } */
    /*     } */

    /*     return NULL; */
    /* } */

    std::ostream& operator<<(std::ostream& stream, const Namespace& self) {
        stream << "<Namespace " << self.name << ">";
        return stream;
    }
}
