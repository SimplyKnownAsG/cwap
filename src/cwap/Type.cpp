#include "cwap/Type.hpp"
#include "cwap/ConvenientClang.hpp"
#include "cwap/Location.hpp"
#include "cwap/Namespace.hpp"
#include "cwap/Project.hpp"
#include "cwap/TypeUsage.hpp"

#include <algorithm>

namespace cwap {

    Type::Type(const string usr, const string name, bool is_basic, bool is_struct, bool is_class)
      : usr(usr)
      , name(name)
      , is_basic(is_basic)
      , is_struct(is_struct)
      , is_class(is_class){};

    /* Type* Type::Create(Project& project, const CXCursor& cursor) { */
    /*     CXType clang_type = clang_getCursorType(cursor); */
    /*     return Type::Create(project, clang_type); */
    /* } */

    Type* Type::Create(Project& project, const CXType& clang_type) {
        if (clang_type.kind == CXType_Invalid) {
            throw std::invalid_argument("Invalid cursor type.");
        }

        CXString type_spelling = clang_getTypeSpelling(clang_type);
        string type_name(clang_getCString(type_spelling));
        clang_disposeString(type_spelling);

        if (project.type_renames.count(type_name) == 1) {
            type_name = project.type_renames.at(type_name);
        }

        CXCursor type_cursor = clang_getTypeDeclaration(clang_type);

        auto result = new Type(get_usr(type_cursor),
                               type_name,
                               CXType_Void < clang_type.kind && clang_type.kind < CXType_NullPtr,
                               type_cursor.kind == CXCursor_StructDecl,
                               type_cursor.kind == CXCursor_ClassDecl);

        const CXCursor& parent = clang_getCursorSemanticParent(type_cursor);

        if ((parent.kind >= CXCursor_FirstInvalid && parent.kind <= CXCursor_LastInvalid) ||
            parent.kind == CXCursor_TranslationUnit) {
            std::string key = result->usr.empty() ? result->name : result->usr;

            if (project._types.count(key) != 1) {
                // XXX: so, this is deleted in Project::get
                project._types[result->name] = result;
            }
        } else if (parent.kind == CXCursor_Namespace) {
            auto space = project.get<Namespace>(parent);
            std::string key = result->usr.empty() ? result->name : result->usr;

            if (space->_types.count(key) != 1) {
                // XXX: so, this is deleted in Project::get
                space->_types[result->name] = result;
            }
        }

        return result;
    }

    CXChildVisitResult Type::visit(CXCursor& cursor, Project& project) {
        Location location = Location::Create(cursor);
        CXCursorKind cursor_kind = clang_getCursorKind(cursor);

        if (!clang_isDeclaration(cursor_kind)) {
            return CXChildVisit_Continue;
        }

        switch (clang_getCXXAccessSpecifier(cursor)) {
        case CX_CXXInvalidAccessSpecifier:
        case CX_CXXPrivate:
            return CXChildVisit_Continue;
        case CX_CXXProtected:
        case CX_CXXPublic:
            // do nothing;
            break;
        }

        switch (cursor.kind) {
        case CXCursor_FieldDecl: {
            TypeUsage* cv = project.get<TypeUsage>(cursor);
            this->_attributes[cv->name] = cv;
            break;
        }
        case CXCursor_CXXMethod: {
            auto cf = project.get<Function>(cursor);
            this->_methods.insert(cf);
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

    const std::unordered_map<string, Type*> Type::types() const {
        return this->_types;
    }

    const std::unordered_map<string, TypeUsage*> Type::attributes() const {
        return this->_attributes;
    }

    std::vector<Function*> Type::methods() const {
        std::vector<Function*> funcs(this->_methods.begin(), this->_methods.end());
        return funcs;
    }

    std::ostream& operator<<(std::ostream& stream, const Type& self) {
        stream << "<Type (" << self.usr << ") ";

        if (self.is_basic) {
            stream << "basic type ";
        }
        if (self.is_struct) {
            stream << "struct ";
        }
        if (self.is_class) {
            stream << "class ";
        }
        stream << self.name << ">";
        return stream;
    }

    void Type::write_header(std::ostream& stream, std::string indent) const {

        if (this->is_basic) {
            return;
        }

        std::string short_name = this->name.substr(this->name.find_last_of(":") + 1);
        if (this->is_class) {
            stream << indent << "class " << short_name << " {" << std::endl;
        } else if (this->is_struct) {
            stream << indent << "struct " << short_name << " {" << std::endl;
        }

        auto sub_indent = indent + "    ";
        for (auto func : this->methods()) {
            func->write_header(stream, sub_indent);
        }

        for (auto name_variable : this->attributes()) {
            stream << indent << "    " << name_variable.second->cwap_type->name << " "
                   << name_variable.first << ";" << std::endl;
        }

        for (auto name_type : this->types()) {
            name_type.second->write_header(stream, sub_indent);
        }

        stream << indent << "};" << std::endl;
    }
}
