#include "tests/cpp/TempFile.hpp"

#include "cwap/cwap.hpp"

#include "catch.hpp"

TEST_CASE("namespaces with builtin types", "[namespaces]") {
    cwap::Project proj("TestFunctions");
    REQUIRE(0 == proj.types().size());
    TempFile temp_file;

    SECTION("int in two namespaces") {
        temp_file << "int a;" << std::endl;
        temp_file << "namespace b {" << std::endl;
        temp_file << "    int a;" << std::endl;
        temp_file << "}" << std::endl;
        temp_file.close();
        proj.parse(temp_file.name);

        REQUIRE(1 == proj.types().size());
    }
}
