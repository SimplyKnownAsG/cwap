#include "cwap/cwap.hpp"

#include <cinttypes>
#include <iostream>
#include <sstream>

void first() {
    return;
}

int second() {
    return 1;
}

int third(int a, int b) {
    return a + b;
}

int third(int a, int b, int c) {
    return a + b + c;
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
        cwap::Project proj("TestFunctions");
        proj.parse("tests/cpp/TestFunctions.cpp");

        {
            cwap::CwapFunction& f = *proj.functions().at("first");
            ASSERT_EQ("first", f.name);
        }
    } catch (std::string& err_str) {
        std::cout << "Test failed. error: " << err_str << std::endl;
    }
}
