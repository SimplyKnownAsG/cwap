#pragma once

#include "cwap/Function.hpp"
#include "cwap/Type.hpp"
#include "cwap/TypeUsage.hpp"

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace cwap {

    class Namespace;
    class Project;

    namespace internal {
        class ClangVisitor;
        class Factory;
    }

    class Namespace {
    private:
        friend class internal::ClangVisitor;
        friend class internal::Factory;
        friend class Type;

        std::unordered_map<std::string, Type*> _types;

        std::unordered_map<std::string, TypeUsage*> _variables;

        std::unordered_set<Function*> _functions;

        std::unordered_map<std::string, Namespace*> _namespaces;

    protected:
        Namespace(const std::string usr, const std::string name);

    public:
        ~Namespace() = default;

        const std::string usr;

        const std::string name;

        const std::unordered_map<std::string, Type*> types() const;

        const std::unordered_map<std::string, TypeUsage*> variables() const;

        std::vector<Function*> functions() const;

        const std::unordered_map<std::string, Namespace*> namespaces() const;

        virtual void write_header(std::ostream& stream, std::string indent = "") const;

        friend std::ostream& operator<<(std::ostream& stream, const Namespace& self);
    };
}
