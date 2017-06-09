#include "cwap/cwap.hpp"

#include <cinttypes>
#include <iostream>
#include <sstream>

int an_integer;

int a_second_integer;

int64_t an_int64t;

long a_long;

long long a_long_long;

float a_float;

double a_double;

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
        cwap::Project proj("TestBasicTypes");
        ASSERT_EQ(0, proj.types().size());
        proj.parse("tests/cpp/TestBasicTypes.cpp");
        ASSERT_EQ(8, proj.types().size());

        {
            cwap::CwapVariable& v = *proj.variables().at("an_integer");
            ASSERT_EQ("an_integer", v.name);
            cwap::CwapType& t = *v.cwap_type;
            ASSERT_EQ("int", t.name);
            ASSERT_EQ(true, t.is_basic);
            ASSERT_EQ(&t, proj.types().at("int"));
        }
        /* { */
        /*     auto v = proj.variable("a_second_integer"); */
        /*     ASSERT_EQ("a_second_integer", v.name); */
        /*     cwap::CwapType& t = *v.cwap_type; */
        /*     ASSERT_EQ(&t, &proj["int"]); */
        /* } */
        /* { */
        /*     auto v = proj.variable("an_int64t"); */
        /*     ASSERT_EQ("an_int64t", v.name); */
        /*     cwap::CwapType& t = *v.cwap_type; */
        /*     ASSERT_EQ(false, t.is_basic); */
        /*     ASSERT_EQ(&t, &proj["int64_t"]); */
        /*     ASSERT_NE(&t, &proj["int"]); */
        /* } */
        /* { */
        /*     auto v = proj.variable("a_long"); */
        /*     ASSERT_EQ("a_long", v.name); */
        /*     cwap::CwapType& t = *v.cwap_type; */
        /*     ASSERT_EQ(true, t.is_basic); */
        /*     ASSERT_EQ(&t, &proj["long"]); */
        /*     ASSERT_NE(&t, &proj["int"]); */
        /*     ASSERT_NE(&t, &proj["int64_t"]); */
        /* } */
        /* { */
        /*     auto v = proj.variable("a_long_long"); */
        /*     ASSERT_EQ("a_long_long", v.name); */
        /*     cwap::CwapType& t = *v.cwap_type; */
        /*     ASSERT_EQ(true, t.is_basic); */
        /*     ASSERT_EQ(&t, &proj["long long"]); */
        /*     ASSERT_NE(&t, &proj["long"]); */
        /*     ASSERT_NE(&t, &proj["int"]); */
        /*     ASSERT_NE(&t, &proj["int64_t"]); */
        /* } */
        /* { */
        /*     auto v = proj.variable("a_float"); */
        /*     ASSERT_EQ("a_float", v.name); */
        /*     cwap::CwapType& t = *v.cwap_type; */
        /*     ASSERT_EQ(true, t.is_basic); */
        /*     ASSERT_EQ(&t, &proj["float"]); */
        /*     ASSERT_NE(&t, &proj["long long"]); */
        /*     ASSERT_NE(&t, &proj["long"]); */
        /*     ASSERT_NE(&t, &proj["int"]); */
        /*     ASSERT_NE(&t, &proj["int64_t"]); */
        /* } */
        /* { */
        /*     auto v = proj.variable("a_double"); */
        /*     ASSERT_EQ("a_double", v.name); */
        /*     cwap::CwapType& t = *v.cwap_type; */
        /*     ASSERT_EQ(true, t.is_basic); */
        /*     ASSERT_EQ(&t, &proj["double"]); */
        /*     ASSERT_NE(&t, &proj["long long"]); */
        /*     ASSERT_NE(&t, &proj["long"]); */
        /*     ASSERT_NE(&t, &proj["int"]); */
        /*     ASSERT_NE(&t, &proj["int64_t"]); */
        /* } */
    } catch (std::string& err_str) {
        std::cout << "Test failed. error: " << err_str << std::endl;
    }
}
