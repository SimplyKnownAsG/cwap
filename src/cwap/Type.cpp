#include "cwap/Type.hpp"
#include "cwap/Access.hpp"
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

    const std::unordered_map<string, Type*> Type::types() const {
        return this->_types;
    }

    const std::vector<TypeUsage*> Type::attributes() const {
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

        // the actual name is Namespace::ParentClass::Name
        std::string short_name = this->name.substr(this->name.find_last_of(":") + 1);
        if (this->is_class) {
            stream << indent << "class " << short_name;
        } else if (this->is_struct) {
            stream << indent << "struct " << short_name;
        }

        if (this->_bases.size() > 0) {
            stream << " : ";
        }
        for (auto ii = 0; ii < this->_bases.size();) {
            Access a;
            Type* base;
            std::tie(a, base) = this->_bases[ii];
            stream << a << " " << base->name;
            if (++ii < this->_bases.size()) {
                stream << ", ";
            }
        }
        stream << " {\n";

        auto sub_indent = indent + "    ";
        for (auto func : this->methods()) {
            func->write_header(stream, sub_indent);
        }

        int blemish_num = 0;
        for (auto attr : this->attributes()) {
            if (attr->access == Access::PRIVATE) {
                stream << sub_indent << attr->access << ": char blemish" << blemish_num++;

                if (attr->size < sizeof(char)) {
                    stream << " : " << attr->size << ";" << std::endl;
                } else {
                    stream << "[" << attr->size / sizeof(char) << "];" << std::endl;
                }
            } else {
                stream << sub_indent << attr->access << ": " << attr->cwap_type->name << " "
                       << attr->name << ";" << std::endl;
            }
        }

        for (auto name_type : this->types()) {
            name_type.second->write_header(stream, sub_indent);
        }

        stream << indent << "};" << std::endl;
    }
}
