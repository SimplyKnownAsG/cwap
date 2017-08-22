#include "cwap/Function.hpp"
#include "cwap/ConvenientClang.hpp"
#include "cwap/Namespace.hpp"
#include "cwap/Parameter.hpp"

#include <iostream>

namespace cwap {

    Function::Function(std::string name, Namespace const* space, const Type* bound_parent)
      : name(name)
      , space(space)
      , bound_parent(bound_parent){};

    Function* Function::Factory(CXCursor& cursor, Namespace* space, Type* bound_parent) {
        Function* func = new Function(get_name(cursor), space, bound_parent);
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

    /* std::ostream& operator<<(std::ostream& stream, const Function& self) { */
    /*     stream << "<Function "; */

    /*     if (self.is_static) { */
    /*         stream << "static "; */
    /*     } */
    /*     if (self.is_basic) { */
    /*         stream << "basic type "; */
    /*     } */
    /*     if (self.is_function) { */
    /*         stream << "function "; */
    /*     } */
    /*     if (self.is_struct) { */
    /*         stream << "struct "; */
    /*     } */
    /*     if (self.is_class) { */
    /*         stream << "class "; */
    /*     } */
    /*     stream << self.name << ">"; */
    /*     return stream; */
    /* } */
}
