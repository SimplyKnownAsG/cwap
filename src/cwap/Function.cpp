#include "cwap/Function.hpp"
#include "cwap/Namespace.hpp"
#include "cwap/Project.hpp"
#include "cwap/TypeUsage.hpp"

namespace cwap {

    Function* Function::Create(Project& project, const CXCursor& cursor) {
        CXType clang_func_type = clang_getCursorType(cursor);

        CXCursor type_decl = clang_getTypeDeclaration(clang_getResultType(clang_func_type));
        Type* t = project.get(clang_getResultType(clang_func_type));
        std::vector<TypeUsage*> params;
        auto func = new Function(t, get_name(cursor), get_usr(cursor));

        for (int ii = 0; ii < clang_getNumArgTypes(clang_func_type); ii++) {
            CXCursor argument_cursor = clang_Cursor_getArgument(cursor, ii);
            func->_parameters.push_back(TypeUsage::Create(project, argument_cursor));
        }

        return func;
    }

    void Function::write_header(std::ostream& stream, std::string indent) const {
        stream << indent << this->return_type->name << " " << this->name << "(";

        for (auto param = this->parameters().begin(), e = this->parameters().end(); param != e;
             param++) {
            auto p = *param;
            stream << p->cwap_type->name;
            stream << " " << p->name;

            if (param != e) {
                stream << "," << std::endl;
            }
        }

        stream << ");" << std::endl;
    }

    std::ostream& operator<<(std::ostream& stream, const Function& self) {
        stream << "<Function ";

        stream << *(self.return_type);
        stream << self.name;
        stream << "(";

        for (auto param : self._parameters) {
            stream << *param;
            stream << ", ";
        }
        stream << self.name << ")>";
        return stream;
    }
}
