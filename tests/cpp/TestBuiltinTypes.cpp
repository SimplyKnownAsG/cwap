#include "TempFile.hpp"

#include "cwap/Project.hpp"

#include "catch.hpp"

TEST_CASE("builtin basic types", "[basictypes]") {
    cwap::Project proj("TestBasicTypes");
    REQUIRE(0 == proj.types().size());
    TempFile temp_file;

    SECTION("an integer") {
        temp_file << "int an_integer;" << std::endl;
        temp_file.close();
        proj.parse(temp_file.name);

        cwap::Variable& v = *proj.variables().at("an_integer");
        REQUIRE("an_integer" == v.name);
        cwap::Type& t = *v.cwap_type;
        REQUIRE("int" == t.name);
        REQUIRE(t.is_basic);
        REQUIRE(&t == proj.types().at("int"));
    }
    SECTION("a float") {
        temp_file << "float a_float;" << std::endl;
        temp_file.close();
        proj.parse(temp_file.name);

        cwap::Variable& v = *proj.variables().at("a_float");
        REQUIRE("a_float" == v.name);
        cwap::Type& t = *v.cwap_type;
        REQUIRE("float" == t.name);
        REQUIRE(t.is_basic);
        REQUIRE(&t == proj.types().at("float"));
    }
    SECTION("a long") {
        temp_file << "long a_long;" << std::endl;
        temp_file.close();
        proj.parse(temp_file.name);

        cwap::Variable& v = *proj.variables().at("a_long");
        REQUIRE("a_long" == v.name);
        cwap::Type& t = *v.cwap_type;
        REQUIRE("long" == t.name);
        REQUIRE(t.is_basic);
        REQUIRE(&t == proj.types().at("long"));
    }
    SECTION("a long long") {
        temp_file << "long long a_long_long;" << std::endl;
        temp_file.close();
        proj.parse(temp_file.name);

        cwap::Variable& v = *proj.variables().at("a_long_long");
        REQUIRE("a_long_long" == v.name);
        cwap::Type& t = *v.cwap_type;
        REQUIRE("long long" == t.name);
        REQUIRE(t.is_basic);
        REQUIRE(&t == proj.types().at("long long"));
    }
    SECTION("a double") {
        temp_file << "double a_double;" << std::endl;
        temp_file.close();
        proj.parse(temp_file.name);

        cwap::Variable& v = *proj.variables().at("a_double");
        REQUIRE("a_double" == v.name);
        cwap::Type& t = *v.cwap_type;
        REQUIRE("double" == t.name);
        REQUIRE(t.is_basic);
        REQUIRE(&t == proj.types().at("double"));
    }

    REQUIRE(proj.types().size() == 1);
    REQUIRE(proj.variables().size() == 1);
}

TEST_CASE("multiple variables with same type", "[basictypes]") {
    cwap::Project proj("TestBasicTypes");
    REQUIRE(0 == proj.types().size());
    TempFile temp_file;

    SECTION("declared in same line") {
        temp_file << "int a, b;" << std::endl;
    }
    SECTION("declared separately") {
        temp_file << "int a;" << std::endl;
        temp_file << "int b;" << std::endl;
    }

    temp_file.close();
    proj.parse(temp_file.name);
    cwap::Variable& var_a = *proj.variables().at("a");
    cwap::Variable& var_b = *proj.variables().at("b");
    REQUIRE(var_a.cwap_type == var_b.cwap_type); // pointer comparison
    REQUIRE(var_a.cwap_type == proj.types().at("int"));
}

TEST_CASE("builtin basic types in one translation unit", "[basictypes]") {
    cwap::Project proj("TestBasicTypes");
    REQUIRE(0 == proj.types().size());
    TempFile temp_file;

    temp_file << "int an_integer;" << std::endl;
    temp_file << "float a_float;" << std::endl;
    temp_file << "long a_long;" << std::endl;
    temp_file << "long long a_long_long;" << std::endl;
    temp_file << "double a_double;" << std::endl;
    temp_file.close();
    proj.parse(temp_file.name);

    REQUIRE(proj.types().size() == 5);
    REQUIRE(proj.variables().size() == 5);

    cwap::Variable& v_int = *proj.variables().at("an_integer");
    cwap::Type& t_int = *v_int.cwap_type;
    REQUIRE(&t_int == proj.types().at("int"));

    cwap::Variable& v_float = *proj.variables().at("a_float");
    cwap::Type& t_float = *v_float.cwap_type;
    REQUIRE(&t_float == proj.types().at("float"));
    REQUIRE(&t_float != &t_int);

    cwap::Variable& v_long = *proj.variables().at("a_long");
    cwap::Type& t_long = *v_long.cwap_type;
    REQUIRE(&t_long == proj.types().at("long"));
    REQUIRE(&t_long != &t_int);
    REQUIRE(&t_long != &t_float);

    cwap::Variable& v_long_long = *proj.variables().at("a_long_long");
    cwap::Type& t_long_long = *v_long_long.cwap_type;
    REQUIRE(&t_long_long == proj.types().at("long long"));
    REQUIRE(&t_long_long != &t_int);
    REQUIRE(&t_long_long != &t_float);
    REQUIRE(&t_long_long != &t_long);

    cwap::Variable& v_double = *proj.variables().at("a_double");
    cwap::Type& t_double = *v_double.cwap_type;
    REQUIRE(&t_double == proj.types().at("double"));
    REQUIRE(&t_double != &t_int);
    REQUIRE(&t_double != &t_float);
    REQUIRE(&t_double != &t_long);
    REQUIRE(&t_double != &t_long_long);
}
