#include "cwap/internal/ClangVisitor.hpp"
#include "cwap/Location.hpp"
#include "cwap/Namespace.hpp"
#include "cwap/Project.hpp"
#include "cwap/Type.hpp"
#include "cwap/internal/ConvenientClang.hpp"
#include "cwap/internal/Factory.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using std::ifstream;
using std::string;
using std::vector;

namespace cwap {
    namespace internal {

        CXChildVisitResult ClangVisitor::VisitChildrenCallback(CXCursor cursor,
                                                               CXCursor parent,
                                                               CXClientData client_data) {
            (void)parent; // suppress -Wunused-parameter

            struct ClangVisitorData* visitor_data = (struct ClangVisitorData*)client_data;

            // extract the data.
            Factory* factory = visitor_data->factory;
            Project* project = visitor_data->project;
            Namespace* space = visitor_data->space; // space may == project
            Type* type = visitor_data->current_type;

            if (!visitor_data) {
                throw std::runtime_error("bad data passed to VisitChildrenCallback");
            }

            Location location = Location::Create(cursor);

            if (!project->sources().count(location.file_name)) {
                return CXChildVisit_Continue;
            }

            // I tried to organize these by similarity.
            switch (cursor.kind) {
            case CXCursor_VarDecl: {
                auto variable = factory->get_type_usage(cursor);
                space->_variables[variable->name] = variable;
                break;
            }
            case CXCursor_FieldDecl: {
                auto* cv = factory->get_type_usage(cursor);
                type->_attributes.push_back(cv);
                break;
            }
            case CXCursor_FunctionDecl: {
                auto cf = factory->get_function(cursor);
                space->_functions.insert(cf);
                break;
            }
            case CXCursor_FunctionTemplate: {
                // source code ends before last semi-colon, so add one
                auto source_code = get_source_code(cursor) + ";";
                auto cf = factory->get_function(cursor, source_code);
                space->_functions.insert(cf);
                break;
            }
            case CXCursor_CXXMethod: {
                auto cf = factory->get_function(cursor);
                type->_methods.push_back(cf);
                break;
            }
            case CXCursor_ClassDecl:
            case CXCursor_StructDecl: {
                Type* new_type = factory->get_type(clang_getCursorType(cursor));
                if (type != nullptr) {
                    type->_types[new_type->name] = new_type;
                } else {
                    space->_types[new_type->name] = new_type;
                }

                // recursive
                struct ClangVisitorData data {
                    factory, project, space, new_type
                };
                clang_visitChildren(cursor, ClangVisitor::VisitChildrenCallback, &data);

                break;
            }
            case CXCursor_CXXBaseSpecifier: {
                // class Name : the_base
                //            ^^^^^^^^^^
                auto base = std::make_tuple(get_access(cursor),
                                            factory->get_type(clang_getCursorType(cursor)));
                type->_bases.push_back(base);
                break;
            }
            case CXCursor_Namespace: {
                Namespace* sub_space = factory->get_namespace(cursor);
                space->_namespaces[sub_space->name] = sub_space;

                // recursive!
                struct ClangVisitorData data {
                    factory, project, sub_space, nullptr
                };
                clang_visitChildren(cursor, ClangVisitor::VisitChildrenCallback, &data);

                break;
            }
            default: {
                /* CXString cursor_kind_name =
                 * clang_getCursorKindSpelling(cursor.kind);
                 */
                /* std::cerr << "I do not know how to
                 * interpret declaration of " */
                /*           <<
                 * clang_getCString(cursor_kind_name) << " ("
                 * << cursor.kind << ")" */
                /*           << std::endl; */
                /* clang_disposeString(cursor_kind_name); */
                break;
            }
            }

            return CXChildVisit_Continue;
        }
    }
}
