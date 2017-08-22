#include "tests/cpp/TempFile.hpp"

#include "cwap/cwap.hpp"

#include "catch.hpp"

TEST_CASE("functions", "[functions]") {
    cwap::Project proj("TestFunctions");
    REQUIRE(0 == proj.types().size());
    TempFile temp_file;

    SECTION("void (void)") {
        temp_file << "void first() { return; } ;" << std::endl;
        temp_file.close();
        proj.parse(temp_file.name);

        cwap::Function& func = *proj.functions().at("first");
        REQUIRE("first" == func.name);
        REQUIRE(proj.types().at("void") == func.return_type);
        REQUIRE(func.parameters().size() == 0);
    }
    SECTION("void (int)") {
        temp_file << "void func(int a) { return; } ;" << std::endl;
        temp_file.close();
        proj.parse(temp_file.name);

        cwap::Function& func = *proj.functions().at("func");
        REQUIRE(proj.types().at("void") == func.return_type);
        REQUIRE(func.parameters().size() == 1);
        auto param_a = func.parameters()[0];
        REQUIRE(param_a->name == "a");
    }
    SECTION("int (void)") {
        temp_file << "int first() { return 1; }" << std::endl;
        temp_file.close();
        proj.parse(temp_file.name);

        cwap::Function& func = *proj.functions().at("first");
        REQUIRE("first" == func.name);
        REQUIRE(proj.types().at("int") == func.return_type);
        REQUIRE(func.parameters().size() == 0);
    }
}
