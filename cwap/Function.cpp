#include "cwap/Function.hpp"
#include "cwap/ConvenientClang.hpp"
#include "cwap/Namespace.hpp"
#include "cwap/Parameter.hpp"

#include <iostream>

namespace cwap {

    Function::Function(std::string name,
                       std::string usr,
                       Namespace const* space,
                       const Type* bound_parent)
      : name(name)
      , usr(usr)
      , space(space)
      , bound_parent(bound_parent){};

    Function* Function::Factory(CXCursor& cursor, Namespace* space, Type* bound_parent) {
        std::string usr = get_usr(cursor);

        for (auto existing_func : space->functions()) {
            // TODO: slow
            if (existing_func->usr == usr) {
                return (Function*)NULL;
            }
        }

        Function* func = new Function(get_name(cursor), get_usr(cursor), space, bound_parent);
        CXType clang_func_type = clang_getCursorType(cursor);
        CXType clang_result_type = clang_getResultType(clang_func_type);
        func->return_type = space->get_type(clang_result_type);
        for (int ii = 0; ii < clang_getNumArgTypes(clang_func_type); ii++) {
            CXCursor argument_cursor = clang_Cursor_getArgument(cursor, ii);
            func->_parameters.push_back(
              Parameter::Factory(argument_cursor, space->get_type(argument_cursor), func));
        }
        return func;
    }

    Function* Function::Factory(CXCursor& cursor, Namespace* space) {
        return Function::Factory(cursor, space, NULL);
    }

    const std::vector<Parameter*> Function::parameters() const {
        return this->_parameters;
    }
}
