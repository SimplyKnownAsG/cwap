#pragma once

#include "cwap/Function.hpp"
#include "cwap/Namespace.hpp"
#include "cwap/Project.hpp"
#include "cwap/Type.hpp"
#include "cwap/TypeUsage.hpp"

#include <clang-c/Index.h>

namespace cwap {
    namespace internal {

        class Factory {
        private:
            Project* project;

            // this is an overzealous collection that should probablye be made more selective
            std::unordered_map<string, void*> _cache;

        public:
            Factory(Project* project);

            Namespace* get_namespace(CXCursor const& cursor);

            Function* get_function(CXCursor const& cursor);

            TypeUsage* get_type_usage(CXCursor const& cursor);

            Type* get_type(CXType const& cxtype);
        };

    }
}
