#include "cwap/Type.hpp"

namespace cwap {
    namespace interfaces {

        class Interface {
        private:
            enum {
                UNCHECKED,
                IMPLEMENTED,
                NOT_IMPLEMENTED

            } _implemented;

        public:
            Interface(Type const& t);

            bool implemented();

        protected:
            Type const& _type;

            virtual bool check_implementation() = 0;
        };
    }
}
