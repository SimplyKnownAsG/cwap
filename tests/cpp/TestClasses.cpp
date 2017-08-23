#include "tests/cpp/TempFile.hpp"

#include "cwap/cwap.hpp"

#include "catch.hpp"

TEST_CASE("empty class and struct", "[classes]") {
    cwap::Project proj("TestClasses");
    REQUIRE(0 == proj.types().size());
    TempFile temp_file;

    cwap::Type* type = NULL;
    SECTION("empty class") {
        temp_file << "class A {} ;" << std::endl;
        temp_file.close();
        proj.parse(temp_file.name);

        type = proj.types().at("A");
        REQUIRE(type->is_class);
        REQUIRE(!type->is_struct);
    }
    SECTION("empty struct") {
        temp_file << "struct A {} ;" << std::endl;
        temp_file.close();
        proj.parse(temp_file.name);

        type = proj.types().at("A");
        REQUIRE(!type->is_class);
        REQUIRE(type->is_struct);
    }

    REQUIRE(!type->is_basic);
    REQUIRE("A" == type->name);
    REQUIRE(1 == proj.types().size());
}

TEST_CASE("class methods", "[classes]") {
    cwap::Project proj("TestClasses");
    REQUIRE(0 == proj.types().size());
    TempFile temp_file;

    temp_file << R"SOURCE(
class A {
public:
    int int_method();
    float float_method();
};
)SOURCE";

    temp_file.close();
    proj.parse(temp_file.name);

    SECTION("method returning int") {
        cwap::Type* a_type = proj.types().at("A");
        cwap::Function* int_method = a_type->methods().at("int_method");
        REQUIRE("int_method" == int_method->name);
        auto return_type = int_method->return_type;
        auto int_type = proj.types().at("int");
        REQUIRE(return_type == int_type);
    }
    SECTION("method returning float") {
        cwap::Type* a_type = proj.types().at("A");
        cwap::Function* float_method = a_type->methods().at("float_method");
        REQUIRE("float_method" == float_method->name);
        auto return_type = float_method->return_type;
        auto float_type = proj.types().at("float");
        REQUIRE(return_type == float_type);
    }
}

// TODO: private methods, attributes
// TODO: protected methods, attributes
