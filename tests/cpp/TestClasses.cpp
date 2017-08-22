#include "tests/cpp/TempFile.hpp"

#include "cwap/cwap.hpp"

#include "catch.hpp"

TEST_CASE("classes", "[classes]") {
    cwap::Project proj("TestClasses");
    REQUIRE(0 == proj.types().size());
    TempFile temp_file;

    SECTION("empty class") {
        temp_file << "class A {} ;" << std::endl;
        temp_file.close();
        proj.parse(temp_file.name);

        cwap::Type& type = *proj.types().at("A");
        REQUIRE("A" == type.name);
    }

    REQUIRE(1 == proj.types().size());
}
