#include "cwap/cwap.hpp"

#include "catch.hpp"

TEST_CASE("parse own project", "[!mayfail]") {
    cwap::Project proj("cwap");
    std::vector<std::string> clang_args{ "-I." };
    proj.parse("cwap/Project.hpp", clang_args);

    SECTION("project") {
        REQUIRE(1 == proj.namespaces().size());
    }
}
