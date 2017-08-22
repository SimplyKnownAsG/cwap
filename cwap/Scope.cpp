#include "cwap/Scope.hpp"
#include "cwap/Location.hpp"

#include <algorithm>
#include <string>

namespace cwap {

    CXChildVisitResult Scope::VisitChildrenCallback(CXCursor cursor,
                                                    CXCursor parent,
                                                    CXClientData client_data) {
        Scope* space = (Scope*)client_data;
        return space->visit(cursor, parent);
    }

    CXChildVisitResult Scope::visit(CXCursor cursor, CXCursor parent) {
        Location location = Location::Create(cursor);
        CXCursorKind cursor_kind = clang_getCursorKind(cursor);
        if (!clang_isDeclaration(cursor_kind)) {
            return CXChildVisit_Recurse;
        }
        switch (cursor.kind) {
            {
            case CXCursor_VarDecl:
                Variable* cv = Variable::Factory(cursor, this);
                this->_variables[cv->name] = cv;
                break;
            }
            {
            case CXCursor_FunctionDecl:
                Function* cf = Function::Factory(cursor, this);
                this->_functions[cf->name] = cf;
                break;
            }
            {
            case CXCursor_ClassDecl:
                Function* cf = Function::Factory(cursor, this);
                this->_functions[cf->name] = cf;
                break;
            }
            {
            default:
                CXString kind_spelling = clang_getCursorKindSpelling(cursor.kind);
                std::cerr << "I do not know how to interpret declaration of "
                          << clang_getCString(kind_spelling) << " (" << cursor.kind << ")"
                          << std::endl;
            }
        }

        // visit children recursively
        clang_visitChildren(cursor, Scope::VisitChildrenCallback, this);

        return CXChildVisit_Recurse;
    }

    Scope::Scope(std::string name)
      : name(name) {}

    Type* Scope::get_type(CXType& clang_type) {
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

    const std::unordered_map<std::string, Type*> Scope::types() const {
        return this->_types;
    }

    const std::unordered_map<std::string, Variable*> Scope::variables() const {
        return this->_variables;
    }

    const std::unordered_map<std::string, Function*> Scope::functions() const {
        return this->_functions;
    }
}
