#include "cwap/Namespace.hpp"
#include "cwap/Location.hpp"

#include <algorithm>
#include <string>

namespace cwap {

    CXChildVisitResult Namespace::VisitChildrenCallback(CXCursor cursor,
                                                        CXCursor parent,
                                                        CXClientData client_data) {
        Namespace* space = (Namespace*)client_data;
        Location location = Location::Create(cursor);
        CXCursorKind cursor_kind = clang_getCursorKind(cursor);
        if (!clang_isDeclaration(cursor_kind)) {
            return CXChildVisit_Recurse;
        }
        switch (cursor.kind) {
            {
            case CXCursor_VarDecl:
                Variable* cv = Variable::Factory(cursor, space);
                space->_variables[cv->name] = cv;
                break;
            }
            {
            case CXCursor_FunctionDecl:
                Function* cf = Function::Factory(cursor, space);
                space->_functions[cf->name] = cf;
                break;
            }
            /* { */
            /* case CXCursor_ClassDecl: */
            /*     Function* cf = Function::Factory(cursor, space); */
            /*     space->_functions[cf->name] = cf; */
            /*     break; */
            /* } */
            {
            default:
                CXString kind_spelling = clang_getCursorKindSpelling(cursor.kind);
                std::cerr << "I do not know how to interpret declaration of "
                          << clang_getCString(kind_spelling) << " (" << cursor.kind << ")"
                          << std::endl;
            }
        }

        // visit children recursively
        clang_visitChildren(cursor, Namespace::VisitChildrenCallback, client_data);

        return CXChildVisit_Recurse;
    }

    Namespace::Namespace(std::string name)
      : name(name) {}

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

    const std::unordered_map<std::string, Function*> Namespace::functions() const {
        return this->_functions;
    }

    const std::unordered_map<std::string, Namespace*> Namespace::namespaces() const {
        return this->_namespaces;
    }

    /* std::vector<std::string> Namespace::names() { */
    /*     std::vector<std::string> names; */
    /*     std::transform( */
    /*       this->_types.begin(), */
    /*       this->_types.end(), */
    /*       std::back_inserter(names), */
    /*       [](decltype(this->types)::value_type const& key_val) { return key_val.first; }); */
    /*     return names; */
    /* } */
}
