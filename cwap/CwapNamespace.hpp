#pragma once

#include "cwap/CwapFunction.hpp"
#include "cwap/CwapType.hpp"
#include "cwap/CwapVariable.hpp"

#include <clang-c/Index.h>

#include <string>
#include <unordered_map>

namespace cwap {

    class CwapNamespace;

    class CwapNamespace {
    private:
        std::unordered_map<std::string, CwapType*> _types;

        std::unordered_map<std::string, CwapVariable*> _variables;

        std::unordered_map<std::string, CwapFunction*> _functions;

        std::unordered_map<std::string, CwapNamespace*> _namespaces;

    protected:
        static CXChildVisitResult VisitChildrenCallback(CXCursor cursor,
                                                        CXCursor parent,
                                                        CXClientData client_data);

        CwapNamespace(const std::string name);

    public:
        const std::string name;

        const std::unordered_map<std::string, CwapType*> types() const;

        const std::unordered_map<std::string, CwapVariable*> variables() const;

        const std::unordered_map<std::string, CwapFunction*> functions() const;

        const std::unordered_map<std::string, CwapNamespace*> namespaces() const;
    };
}
