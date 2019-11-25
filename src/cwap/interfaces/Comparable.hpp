#include "cwap/interfaces/Interface.hpp"

namespace cwap {
    namespace interfaces {
        class Comparable : public Interface {
        public:
            Comparable(Type const& t);

        protected:
            bool check_implementation() override;
        };
    }
}
