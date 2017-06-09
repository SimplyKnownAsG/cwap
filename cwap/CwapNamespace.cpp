#include "cwap/CwapNamespace.hpp"
#include "cwap/Location.hpp"

#include <clang-c/Index.h>

#include <algorithm>
#include <iostream>
#include <string>

std::string get_name(const CXCursor& cursor) {
    CXString spell = clang_getCursorSpelling(cursor);
    std::string name(clang_getCString(spell));
    clang_disposeString(spell);
    return name;
}

std::string get_linkage(const CXCursor& cursor) {
    CXLinkageKind linkage = clang_getCursorLinkage(cursor);
    std::string linkageName;
    switch (linkage) {
    case CXLinkage_Invalid:
        linkageName = "Invalid";
        break;
    case CXLinkage_NoLinkage:
        linkageName = "NoLinkage";
        break;
    case CXLinkage_Internal:
        linkageName = "Internal";
        break;
    case CXLinkage_UniqueExternal:
        linkageName = "UniqueExternal";
        break;
    case CXLinkage_External:
        linkageName = "External";
        break;
    default:
        linkageName = "Unknown";
        break;
    }
    return linkageName;
}

void show_parent(const CXCursor& cursor, const CXCursor& parent) {
    CXCursor semaParent = clang_getCursorSemanticParent(cursor);
    CXCursor lexParent = clang_getCursorLexicalParent(cursor);
    CXString parentName = clang_getCursorSpelling(parent);
    CXString semaParentName = clang_getCursorSpelling(semaParent);
    CXString lexParentName = clang_getCursorSpelling(lexParent);
    printf("  Parent: parent:%s semantic:%s lexicial:%s\n",
           clang_getCString(parentName),
           clang_getCString(semaParentName),
           clang_getCString(lexParentName));

    clang_disposeString(parentName);
    clang_disposeString(semaParentName);
    clang_disposeString(lexParentName);
}

void show_usr(const CXCursor& cursor) {
    CXString usr = clang_getCursorUSR(cursor);
    printf("  USR: %s\n", clang_getCString(usr));
    clang_disposeString(usr);
}

bool get_declaration_name(const CXCursor& cursor, std::string& cursor_kind) {
    CXCursorKind curKind = clang_getCursorKind(cursor);

    CXString curKindName = clang_getCursorKindSpelling(curKind);

    cursor_kind = std::string(clang_getCString(curKindName));
    clang_disposeString(curKindName);

    return clang_isDeclaration(curKind);
}

namespace cwap {

    CXChildVisitResult CwapNamespace::VisitChildrenCallback(CXCursor cursor,
                                                            CXCursor parent,
                                                            CXClientData client_data) {
        cwap::CwapNamespace* p = (cwap::CwapNamespace*)client_data;

        std::string name = get_name(cursor);
        Location location = Location::Create(cursor);
        if (location.file_name != "tests/cpp/TestBasicTypes.cpp") {
            std::cout << "leaving for file name: " << location.file_name << std::endl;
            return CXChildVisit_Continue;
        }
        std::string linkage = get_linkage(cursor);
        /* if (linkage != "External") { */
        /* } */
        std::string declaration_type;
        if (!get_declaration_name(cursor, declaration_type)) {
            return CXChildVisit_Recurse;
        }
        cwap::CwapType* ct = CwapType::Factory(cursor, p);
        std::cout << "  Name: " << name << std::endl;
        std::cout << "  Type: " << ct->name << std::endl;
        std::cout << "  Location: " << location << std::endl;
        std::cout << "  Linkage: " << linkage << std::endl;
        std::cout << "  Declaration Type: " << declaration_type << std::endl;
        if (p->_types.count(ct->name)) {
            auto temp = ct;
            ct = p->_types[ct->name];
            delete temp;
        } else {
            p->_types[ct->name] = ct;
        }
        cwap::CwapVariable* cv = new cwap::CwapVariable(name, "", ct);
        p->_variables[cv->name] = cv;
        show_parent(cursor, parent);
        show_usr(cursor);
        std::cout << std::endl;

        // visit children recursively
        clang_visitChildren(cursor, CwapNamespace::VisitChildrenCallback, client_data);

        return CXChildVisit_Recurse;
    }

    CwapNamespace::CwapNamespace(std::string name)
      : name(name) {}

    const std::unordered_map<std::string, CwapType*> CwapNamespace::types() const {
        std::cout << " types size is ... " << this->_types.size() << std::endl;
        return this->_types;
    }

    const std::unordered_map<std::string, CwapVariable*> CwapNamespace::variables() const {
        return this->_variables;
    }

    const std::unordered_map<std::string, CwapFunction*> CwapNamespace::functions() const {
        return this->_functions;
    }

    const std::unordered_map<std::string, CwapNamespace*> CwapNamespace::namespaces() const {
        return this->_namespaces;
    }
    /* std::vector<std::string> CwapNamespace::names() { */
    /*     std::vector<std::string> names; */
    /*     std::transform( */
    /*       this->_types.begin(), */
    /*       this->_types.end(), */
    /*       std::back_inserter(names), */
    /*       [](decltype(this->types)::value_type const& key_val) { return key_val.first; }); */
    /*     return names; */
    /* } */
}
