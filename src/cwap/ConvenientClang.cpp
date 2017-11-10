#include "cwap/ConvenientClang.hpp"
#include "cwap/Function.hpp"
#include "cwap/Parameter.hpp"
#include "cwap/Project.hpp"
#include "cwap/Type.hpp"

namespace cwap {
    std::string get_name(const CXCursor& cursor) {
        CXString spell = clang_getCursorSpelling(cursor);
        std::string name(clang_getCString(spell));
        clang_disposeString(spell);
        return name;
    }

    std::string get_usr(const CXCursor& cursor) {
        CXString spell = clang_getCursorUSR(cursor);
        std::string usr(clang_getCString(spell));
        clang_disposeString(spell);
        return usr;
    }

    Function* Clanger::create_function(const CXCursor& cursor, Project& project) {
        std::string usr = get_usr(cursor);

        if (project.has_function(usr)) {
            return (Function*)NULL;
        }

        CXType clang_func_type = clang_getCursorType(cursor);
        CXType clang_result_type = clang_getResultType(clang_func_type);
        std::vector<Parameter*> params;
        Function* func =
          new Function(project.get_type(clang_result_type), get_name(cursor), get_usr(cursor));
        for (int ii = 0; ii < clang_getNumArgTypes(clang_func_type); ii++) {
            CXCursor argument_cursor = clang_Cursor_getArgument(cursor, ii);
            func->_parameters.push_back(
              Parameter::Factory(argument_cursor, project.get_type(argument_cursor)));
        }

        return func;
    }
}
