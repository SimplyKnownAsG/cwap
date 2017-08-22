#include "cwap/Function.hpp"
#include "cwap/ConvenientClang.hpp"
#include "cwap/Namespace.hpp"

#include <iostream>

namespace cwap {

    Function::Function(std::string name, Namespace const* space)
      : name(name)
      , space(space){};

    Function* Function::Factory(CXCursor& cursor, Namespace* space) {
        Function* func = new Function(get_name(cursor), space);
        CXType clang_func_type = clang_getCursorType(cursor);
        CXType clang_result_type = clang_getResultType(clang_func_type);
        func->return_type = space->get_type(clang_result_type);
        for (int ii = 0; ii < clang_getNumArgTypes(clang_func_type); ii++) {
            CXCursor argument_cursor = clang_Cursor_getArgument(cursor, ii);
            func->_arguments.push_back(Variable::Factory(argument_cursor, space));
        }
        return func;
    }

    const std::vector<Variable*> Function::arguments() const {
        return this->_arguments;
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
