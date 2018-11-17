#pragma once

#include <clang-c/Index.h>

namespace cwap {
    class Project;
    class Namespace;
    class Type;

    namespace internal {

        class ClangVisitor {
        protected:
            friend class ::cwap::Project;
            struct ClangVisitorData {
                Project* project;
                Namespace* space;
                Type* current_type;
            };
            static CXChildVisitResult VisitChildrenCallback(CXCursor cursor,
                                                            CXCursor parent,
                                                            CXClientData client_data);

            CXChildVisitResult visit(CXCursor& cursor, Project* project, Type* current_type);
        };
    }
}
