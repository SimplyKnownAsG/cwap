#include "cwap/internal/Factory.hpp"
#include "cwap/internal/ConvenientClang.hpp"

#include <exception>

using namespace std;

namespace cwap {
    namespace internal {

        Factory::Factory(Project* project)
          : project(project) {}

        Namespace* Factory::get_namespace(CXCursor const& cursor) {
            if (cursor.kind != CXCursor_Namespace) {
                throw std::invalid_argument("Cursor is not CXCursor_Namespace");
            }

            string usr = get_usr(cursor);

            if (this->_cache.count(usr) == 1) {
                return (Namespace*)this->_cache[usr];
            }

            auto space = new Namespace(usr, get_name(cursor));

            const CXCursor& parent = clang_getCursorSemanticParent(cursor);

            // TODO: could hide __cxx11 namespaces here
            if (parent.kind == CXCursor_Namespace) {
                // getting recursive
                auto parent_space = this->get_namespace(parent);
                parent_space->_namespaces[space->name] = space;
            } else {
                this->project->_namespaces[space->name] = space;
            }

            if (usr == "") {
                throw runtime_error("Namespace has unexpected blank usr");
            }
            this->_cache[usr] = space;
            return space;
        }

        Function* Factory::get_function(CXCursor const& cursor, string source_code) {
            string usr = get_usr(cursor);

            if (this->_cache.count(usr) == 1) {
                return (Function*)this->_cache[usr];
            }

            CXType clang_func_type = clang_getCursorType(cursor);

            Type* t = this->get_type(clang_getResultType(clang_func_type));
            std::vector<TypeUsage*> params;
            auto func = new Function(t, get_name(cursor), usr, source_code);

            for (int ii = 0; ii < clang_getNumArgTypes(clang_func_type); ii++) {
                CXCursor argument_cursor = clang_Cursor_getArgument(cursor, ii);
                func->_parameters.push_back(this->get_type_usage(argument_cursor));
            }

            this->_cache[usr] = func;
            return func;
        }

        TypeUsage* Factory::get_type_usage(CXCursor const& cursor) {
            string usr = get_usr(cursor);

            if (this->_cache.count(usr) == 1) {
                return (TypeUsage*)this->_cache[usr];
            }

            auto clang_type = clang_getCursorType(cursor);
            auto type = this->get_type(clang_type);
            int size = clang_Type_getSizeOf(clang_type);
            TypeUsage* var = new TypeUsage(usr, get_name(cursor), get_access(cursor), size, type);

            // usr is "" for template parameters
            if (var->usr != "") {
                this->_cache[usr] = var;
            }

            return var;
        }

        Type* Factory::get_type(CXType const& cxtype) {
            /* if (cxtype.kind == CXType_Invalid) { */
            /*     throw std::invalid_argument("Invalid cursor type Factory::get_type."); */
            /* } */

            auto usr = get_usr(clang_getTypeDeclaration(cxtype));

            CXString type_spelling = clang_getTypeSpelling(cxtype);
            string type_name(clang_getCString(type_spelling));
            clang_disposeString(type_spelling);

            if (this->project->type_renames.count(type_name) == 1) {
                type_name = this->project->type_renames.at(type_name);
            }

            CXCursor type_cursor = clang_getTypeDeclaration(cxtype);

            auto key = usr.empty() ? type_name : usr;
            if (this->_cache.count(key) == 1) {
                return (Type*)this->_cache[key];
            }

            auto result = new Type(usr,
                                   type_name,
                                   CXType_Void < cxtype.kind && cxtype.kind < CXType_NullPtr,
                                   type_cursor.kind == CXCursor_StructDecl,
                                   type_cursor.kind == CXCursor_ClassDecl);

            this->_cache[key] = result;

            {
                // adds to the space->_types because we could be grabbing a type out of order, but
                // we don't want a dangling pointer
                auto space = (Namespace*)this->project;
                const CXCursor& parent = clang_getCursorSemanticParent(type_cursor);
                if (parent.kind == CXCursor_Namespace) {
                    space = this->get_namespace(parent);
                }

                // XXX: key is only the key for the cache.
                space->_types[type_name] = result;
            }

            if (cxtype.kind == CXType_Invalid) {
                std::cout << "invalid with : " << usr << " .. " << type_name << std::endl;
            }

            return result;
        }
    }
}
