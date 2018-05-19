#include "cwap/Type.hpp"
#include "cwap/Attribute.hpp"
#include "cwap/ConvenientClang.hpp"
#include "cwap/Location.hpp"
#include "cwap/Namespace.hpp"
#include "cwap/Project.hpp"

namespace cwap {

    Type::Type(std::string name, bool is_basic, bool is_struct, bool is_class)
      : name(name)
      , is_basic(is_basic)
      , is_struct(is_struct)
      , is_class(is_class){};

    Type* Type::Factory(CXType& type) {
        CXString type_spelling = clang_getTypeSpelling(type);
        std::string type_name(clang_getCString(type_spelling));

        CXType real_type = clang_getCanonicalType(type);

        // try to figure out the namespace when "using namespace std" was used
        if (real_type.kind != CXType_Invalid) {
            CXString real_type_spelling = clang_getTypeSpelling(real_type);
            std::string real_type_name(clang_getCString(real_type_spelling));

            // private namespace "::__" is too far
            size_t private_index = real_type_name.find("::__");
            private_index += private_index == std::string::npos ? 0 : 2;

            // anything in a template is also too far
            size_t template_start = real_type_name.find("<");

            std::string non_private_non_template =
              real_type_name.substr(0, std::min(private_index, template_start));

            size_t namespace_index = non_private_non_template.find("::");
            if (namespace_index != std::string::npos) {
                std::string space_name = non_private_non_template.substr(0, namespace_index + 2);

                // TODO: this might fail for legitmate name::name:: (double namespace)
                // Prevent prefixing namespace when it is already there
                if (type_name.find(space_name) == std::string::npos) {
                    type_name = space_name + type_name;
                }
            }
            clang_disposeString(real_type_spelling);
        }

        clang_disposeString(type_spelling);
        return new Type(
          type_name, CXType_Void < type.kind && type.kind < CXType_NullPtr, false, false);
    }

    Type* Type::Factory(CXCursor& cursor) {
        CXType clang_type = clang_getCursorType(cursor);
        CXString type_spelling = clang_getTypeSpelling(clang_type);
        std::string type_name(clang_getCString(type_spelling));
        clang_disposeString(type_spelling);
        return new Type(
          type_name, false, cursor.kind == CXCursor_StructDecl, cursor.kind == CXCursor_ClassDecl);
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
            Type* attr_type = project.get_type(cursor);
            Attribute* cv = Attribute::Factory(cursor, attr_type);
            this->_attributes[cv->name] = cv;
            break;
        }
        case CXCursor_CXXMethod: {
            Function* cf = Clanger::create_function(cursor, project);
            if (cf != NULL) {
                this->_methods.push_back(cf);
            }
            break;
        }
        case CXCursor_ClassDecl:
        case CXCursor_StructDecl: {
            Type* type = Type::Factory(cursor);
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

    bool Type::has_function(std::string usr) const {
        for (auto method : this->_methods) {
            if (method->usr == usr) {
                return true;
            }
        }
        for (auto name_type : this->_types) {
            if (name_type.second->has_function(usr)) {
                return true;
            }
        }
        return false;
    }

    const std::string Type::get_namespace_name() const {
        std::size_t index = this->name.find(":");
        if (index == std::string::npos) {
            return "";
        }
        return this->name.substr(0, index);
    }
}
