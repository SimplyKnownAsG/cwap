#include "cwap/ClangVisitor.hpp"

#include <algorithm>
#include <string>

namespace cwap {

    CXChildVisitResult ClangVisitor::VisitChildrenCallback(CXCursor cursor,
                                                           CXCursor parent,
                                                           CXClientData client_data) {
        ClangVisitor* visitor = (ClangVisitor*)client_data;
        return visitor->visit(cursor, parent);
    }
}
