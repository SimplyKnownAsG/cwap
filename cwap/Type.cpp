#include "cwap/Type.hpp"
#include "cwap/Attribute.hpp"
#include "cwap/Location.hpp"
#include "cwap/Namespace.hpp"

namespace cwap {

    Type::Type(std::string name,
               Namespace const* space,
               bool is_basic,
               bool is_struct,
               bool is_class)
      : name(name)
      , space(space)
      , is_basic(is_basic)
      , is_struct(is_struct)
      , is_class(is_class){};

    Type* Type::Factory(CXType& type, Namespace const* space) {
        CXString type_spelling = clang_getTypeSpelling(type);
        std::string type_name(clang_getCString(type_spelling));
        clang_disposeString(type_spelling);
        return new Type(
          type_name, space, CXType_Void < type.kind && type.kind < CXType_NullPtr, false, false);
    }

    Type* Type::Factory(CXCursor& cursor, Namespace const* space) {
        CXType clang_type = clang_getCursorType(cursor);
        CXString type_spelling = clang_getTypeSpelling(clang_type);
        std::string type_name(clang_getCString(type_spelling));
        clang_disposeString(type_spelling);
        return new Type(type_name,
                        space,
                        false,
                        cursor.kind == CXCursor_StructDecl,
                        cursor.kind == CXCursor_ClassDecl);
    }

    CXChildVisitResult Type::visit(CXCursor& cursor, CXCursor& parent) {
        Location location = Location::Create(cursor);
        CXCursorKind cursor_kind = clang_getCursorKind(cursor);
        if (!clang_isDeclaration(cursor_kind)) {
            return CXChildVisit_Continue;
        }
        switch (cursor.kind) {

        case CXCursor_FieldDecl: {
            Attribute* cv = Attribute::Factory(cursor, this);
            this->_attributes[cv->name] = cv;
            break;
        }
        case CXCursor_CXXMethod: {
            Function* cf = Clanger::create_function(cursor, const_cast<Namespace*>(this->space));
            if (cf != NULL) {
                this->_methods.push_back(cf);
            }
            break;
        }
        case CXCursor_ClassDecl:
        case CXCursor_StructDecl: {
            Type* type = Type::Factory(cursor, const_cast<Namespace*>(this->space));
            type->parent = this;
            this->_types[type->name] = type;
            // recursive
            clang_visitChildren(cursor, Type::VisitChildrenCallback, type);
            break;
        }
        default: {
            CXString cursor_kind_name = clang_getCursorKindSpelling(cursor.kind);
            std::cerr << "I do not know how to interpret declaration of "
                      << clang_getCString(cursor_kind_name) << " (" << cursor.kind << ")"
                      << std::endl;
            clang_disposeString(cursor_kind_name);
            break;
        }
        }

        return CXChildVisit_Continue;
    }

    const std::unordered_map<std::string, Type*> Type::types() const {
        return this->_types;
    }

    const std::unordered_map<std::string, Attribute*> Type::attributes() const {
        return this->_attributes;
    }

    const std::vector<Function*> Type::methods() const {
        return this->_methods;
    }

    std::ostream& operator<<(std::ostream& stream, const Type& self) {
        stream << "<Type ";

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

    void Type::dump_yaml(std::ostream& stream) const {
        stream << "{" << std::endl;
        stream << "name: '" << this->name << "'," << std::endl;

        stream << "methods: [" << std::endl;
        for (auto method : this->methods()) {
            method->dump_yaml(stream);
            stream << "," << std::endl;
        }
        stream << "]," << std::endl;

        stream << "attributes: [" << std::endl;
        for (auto name_attribute : this->attributes()) {
            name_attribute.second->dump_yaml(stream);
            stream << "," << std::endl;
        }
        stream << "]," << std::endl;

        stream << "types: [" << std::endl;
        for (auto name_type : this->types()) {
            name_type.second->dump_yaml(stream);
            stream << "," << std::endl;
        }
        stream << "]" << std::endl;

        stream << "}" << std::endl;
    }
}
