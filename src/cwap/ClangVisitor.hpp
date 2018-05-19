#pragma once

#include <clang-c/Index.h>

namespace cwap {

    class Project;

    class ClangVisitor {
    protected:
        struct ClangVisitorData {
            ClangVisitor* visitor;
            Project& project;
        };
        static CXChildVisitResult VisitChildrenCallback(CXCursor cursor,
                                                        CXCursor parent,
                                                        CXClientData client_data);

        virtual CXChildVisitResult visit(CXCursor& cursor, Project& project) = 0;
    };
}
