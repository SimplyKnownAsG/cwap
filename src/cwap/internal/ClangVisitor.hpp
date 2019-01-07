#pragma once

#include <clang-c/Index.h>

namespace cwap {
    class Function;
    class Project;
    class Namespace;
    class Type;

    namespace internal {

        class Factory;

        class ClangVisitor {
        protected:
            friend class ::cwap::Project;
            struct ClangVisitorData {
                Factory* factory;
                Project* project;
                Namespace* space;
                Type* current_type;
            };
            static CXChildVisitResult VisitChildrenCallback(CXCursor cursor,
                                                            CXCursor parent,
                                                            CXClientData client_data);

            CXChildVisitResult visit(CXCursor& cursor,
                                     Project* project,
                                     Namespace* space,
                                     Type* current_type);
        };

    }
}
