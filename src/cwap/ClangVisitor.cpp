#include "cwap/ClangVisitor.hpp"

#include <string>

namespace cwap {

    CXChildVisitResult ClangVisitor::VisitChildrenCallback(CXCursor cursor,
                                                           CXCursor parent,
                                                           CXClientData client_data) {
        struct ClangVisitorData* visitor_data = (struct ClangVisitorData*)client_data;
        if (!visitor_data) {
            throw "bad data based to VisitChildrenCallback";
        }
        return visitor_data->visitor->visit(cursor, visitor_data->project);
    }
}
