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

        cwap::Namespace* space = proj.namespaces().at("b");
        REQUIRE(1 == space->types().size());
        REQUIRE("a" == space->variables().at("a")->name);
    }
    SECTION("namespace declared twice") {
        temp_file << "namespace a {" << std::endl;
        temp_file << "    int b;" << std::endl;
        temp_file << "}" << std::endl;
        temp_file << "namespace a {" << std::endl;
        temp_file << "    int c;" << std::endl;
        temp_file << "}" << std::endl;
        temp_file.close();
        proj.parse(temp_file.name);

        REQUIRE(0 == proj.types().size());
        REQUIRE(1 == proj.namespaces().size());
        REQUIRE(0 == proj.variables().size());

        cwap::Namespace* space = proj.namespaces().at("a");
        REQUIRE(1 == space->types().size());
        REQUIRE(2 == space->variables().size());
        REQUIRE("b" == space->variables().at("b")->name);
        REQUIRE("c" == space->variables().at("c")->name);
    }
}
// TODO: namespace alias
