#pragma once

#include <clang-c/Index.h>

namespace cwap {

    class ClangVisitor {
    protected:
        static CXChildVisitResult VisitChildrenCallback(CXCursor cursor,
                                                        CXCursor parent,
                                                        CXClientData client_data);

        virtual CXChildVisitResult visit(CXCursor& cursor, CXCursor& parent) = 0;
    };
}
