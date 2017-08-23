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

        cwap::Function& func = *proj.functions()[0];
        REQUIRE("first" == func.name);
        REQUIRE(proj.types().at("void") == func.return_type);
        REQUIRE(func.parameters().size() == 0);
    }
    SECTION("void (int)") {
        temp_file << "void func(int a) { return; } ;" << std::endl;
        temp_file.close();
        proj.parse(temp_file.name);

        cwap::Function& func = *proj.functions()[0];
        REQUIRE(proj.types().at("void") == func.return_type);
        REQUIRE(func.parameters().size() == 1);
        auto param_a = func.parameters()[0];
        REQUIRE(param_a->name == "a");
    }
    SECTION("int (void)") {
        temp_file << "int first() { return 1; }" << std::endl;
        temp_file.close();
        proj.parse(temp_file.name);

        cwap::Function& func = *proj.functions()[0];
        REQUIRE("first" == func.name);
        REQUIRE(proj.types().at("int") == func.return_type);
        REQUIRE(func.parameters().size() == 0);
    }
    SECTION("declaration and definition do not duplicate") {
        temp_file << R"SOURCE(
int def_decl();
int def_decl() {
    int a = 1;
    int b = 2;
    return a + b * 3;
}
)SOURCE";
        temp_file.close();
        proj.parse(temp_file.name);

        REQUIRE(1 == proj.functions().size());
    }
    SECTION("differing parameter name between declaration and definition") {
        temp_file << R"SOURCE(
int def_decl(int param1);
int def_decl(int param2) {
    return param2 * 3;
}
)SOURCE";
        temp_file.close();
        proj.parse(temp_file.name);

        cwap::Parameter* param = proj.functions()[0]->parameters()[0];
        REQUIRE(param->name == "param1");
    }
}

TEST_CASE("overloaded functions", "[functions]") {
    cwap::Project proj("TestFunctions");
    REQUIRE(0 == proj.types().size());

    TempFile temp_file;
    temp_file << R"SOURCE(
int overloaded();
int overloaded(int param1);
int overloaded(float floaty);
)SOURCE";

    temp_file.close();
    proj.parse(temp_file.name);
    REQUIRE(proj.functions().size() == 3);

    for (auto func : proj.functions()) {
        REQUIRE("overloaded" == func->name);
    }

    SECTION("first overload without parameter") {
        cwap::Function* func = proj.functions()[0];
        REQUIRE(func->parameters().size() == 0);
    }
    SECTION("second overload with int parameter") {
        cwap::Function* func = proj.functions()[1];
        REQUIRE(func->parameters().size() == 1);
        cwap::Parameter* param = func->parameters()[0];
        REQUIRE(param->name == "param1");
        REQUIRE(param->cwap_type == proj.types().at("int"));
    }
    SECTION("third overload with float parameter") {
        cwap::Function* func = proj.functions()[2];
        REQUIRE(func->parameters().size() == 1);
        cwap::Parameter* param = func->parameters()[0];
        REQUIRE(param->name == "floaty");
        REQUIRE(param->cwap_type == proj.types().at("float"));
    }
}
