#include "cwap/cwap.hpp"

#include <cinttypes>
#include <iostream>
#include <sstream>

namespace empty {}

namespace one {
    int a;
    namespace two {
        int a;
        int b;
    }
}

#define ASSERT_EQ(expected, actual)                                                                \
    if (!((expected) == (actual))) {                                                               \
        std::ostringstream err_msg;                                                                \
        err_msg << "Objects not equal. Got (" << actual << "), but expected (" << expected << ")"; \
        throw err_msg.str();                                                                       \
    }

#define ASSERT_NE(expected, actual)                                                                \
    if (!((expected) != (actual))) {                                                               \
        std::ostringstream err_msg;                                                                \
        err_msg << "Objects equal, but should not be. Got (" << actual << ") and (" << expected    \
                << ")";                                                                            \
        throw err_msg.str();                                                                       \
    }

int main(int argc, char* argv[]) {
    try {
        cwap::Project proj("TestSpacenames");
        throw "not working";
        /* ASSERT_EQ(0, proj.size()); */
        /* proj.parse("tests/cpp/TestSpacenames.cpp"); */
        /* auto names = proj.names(); */
        /* std::cout << "There are " << names.size() << " registerd names" << std::endl; */
        /* for (auto name : names) { */
        /*     std::cout << name << std::endl; */
        /* } */
        /* std::cout << "There are " << names.size() << " registerd names" << std::endl; */
        /* ASSERT_EQ(8, proj.size()); */

    } catch (std::string& err_str) {
        std::cout << "Test failed. error: " << err_str << std::endl;
    }
}
