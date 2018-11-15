#include "cwap/Access.hpp"

#include <stdexcept>

namespace cwap {
    Access from_clang(CXCursor const& cursor) {
        return from_clang(clang_getCXXAccessSpecifier(cursor));
    }

    Access from_clang(CX_CXXAccessSpecifier cxx_access) {
        switch (cxx_access) {
        case CX_CXXPublic:
            return Access::PUBLIC;
        case CX_CXXProtected:
            return Access::PROTECTED;
        case CX_CXXPrivate:
            return Access::PRIVATE;
        case CX_CXXInvalidAccessSpecifier:
            // this appears to happen for non-class stuff, like a free function
            return Access::PUBLIC;
        }
    }

    std::ostream& operator<<(std::ostream& stream, const Access& access) {
        switch (access) {
        case cwap::Access::PUBLIC:
            stream << "public: ";
            break;
        case cwap::Access::PRIVATE:
            stream << "private: ";
            break;
        case cwap::Access::PROTECTED:
            stream << "protected: ";
            break;
        case cwap::Access::INTERNAL:
            stream << "internal: ";
            break;
        }
        return stream;
    }
}
