#pragma once

#include "cwap/Access.hpp"
#include "cwap/Function.hpp"

#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using std::string;

namespace cwap {

    class Project;
    class Type;
    class TypeUsage;

    namespace internal {
        class ClangVisitor;
        class Factory;
    }

    class Type {
    private:
        friend class internal::ClangVisitor;
        friend class internal::Factory;

        std::unordered_map<string, Type*> _types;

        std::vector<TypeUsage*> _attributes;

        std::vector<Function*> _methods;

        std::vector<std::tuple<Access, Type*>> _bases;

    public:
        virtual ~Type() = default;

        string const usr;

        string const name;

        bool const is_basic;

        bool const is_struct;

        bool const is_class;

        std::unordered_map<string, Type*> const types() const;

        std::vector<TypeUsage*> const attributes() const;

        std::vector<Function*> methods() const;

        void write_header(std::ostream& stream, std::string indent) const;

        Type(string const usr, string const name, bool is_basic, bool is_struct, bool is_class);

        friend std::ostream& operator<<(std::ostream& stream, const Type& self);
    };
}
