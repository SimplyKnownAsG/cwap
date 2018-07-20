#include "TempFile.hpp"

#include "cwap/cwap.hpp"

#include "catch.hpp"

#include <algorithm>

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
    cwap::Type* a_type = proj.types().at("A");
    auto meths = a_type->methods();

    SECTION("method returning int") {
        /* cwap::Function* int_method = a_type->methods()[0]; */
        cwap::Function* int_method =
                *std::find_if(meths.begin(), meths.end(), [](cwap::Function* m) {
                    return m->name == "int_method";
                });
        REQUIRE("int_method" == int_method->name);
        auto return_type = int_method->return_type;
        auto int_type = proj.types().at("int");
        REQUIRE(return_type == int_type);
    }
    SECTION("method returning float") {
        cwap::Function* float_method =
                *std::find_if(meths.begin(), meths.end(), [](cwap::Function* m) {
                    return m->name == "float_method";
                });
        REQUIRE("float_method" == float_method->name);
        auto return_type = float_method->return_type;
        auto float_type = proj.types().at("float");
        REQUIRE(return_type == float_type);
    }
}

TEST_CASE("overloaded methods", "[classes]") {
    cwap::Project proj("TestClasses");
    REQUIRE(0 == proj.types().size());

    TempFile temp_file;
    temp_file << R"SOURCE(
class A {
public:
    int overloaded();
    int overloaded(int param1);
    int overloaded(float floaty);
};
)SOURCE";

    temp_file.close();
    proj.parse(temp_file.name);
    cwap::Type* a_type = proj.types().at("A");
    REQUIRE(a_type->methods().size() == 3);

    for (auto func : a_type->methods()) {
        REQUIRE("overloaded" == func->name);
    }

    SECTION("first overload without parameter") {
        cwap::Function* func = a_type->methods()[0];
        REQUIRE(func->parameters().size() == 0);
    }
    SECTION("second overload with int parameter") {
        cwap::Function* func = a_type->methods()[1];
        REQUIRE(func->parameters().size() == 1);
        cwap::TypeUsage* param = func->parameters()[0];
        REQUIRE(param->name == "param1");
        REQUIRE(param->cwap_type == proj.types().at("int"));
    }
    SECTION("third overload with float parameter") {
        cwap::Function* func = a_type->methods()[2];
        REQUIRE(func->parameters().size() == 1);
        cwap::TypeUsage* param = func->parameters()[0];
        REQUIRE(param->name == "floaty");
        REQUIRE(param->cwap_type == proj.types().at("float"));
    }
}

TEST_CASE("attributes", "[classes]") {
    cwap::Project proj("TestClasses");
    REQUIRE(0 == proj.types().size());

    TempFile temp_file;
    temp_file << R"SOURCE(
class A {
public:
    int available_to_all;
protected:
    int available_to_children;
private:
    int hidden;
};
)SOURCE";

    temp_file.close();
    proj.parse(temp_file.name);
    cwap::Type* a_type = proj.types().at("A");

    SECTION("public attribute") {
        cwap::TypeUsage* attr = a_type->attributes().at("available_to_all");
        REQUIRE(attr->name == "available_to_all");
        REQUIRE(attr->cwap_type == proj.types().at("int"));
    }
    SECTION("second overload with int parameter") {
        cwap::TypeUsage* attr = a_type->attributes().at("available_to_children");
        REQUIRE(attr->name == "available_to_children");
        REQUIRE(attr->cwap_type == proj.types().at("int"));
    }
    SECTION("third overload with float parameter") {
        REQUIRE_THROWS(a_type->attributes().at("hidden"));
    }
}

TEST_CASE("nested classses", "[classes]") {
    cwap::Project proj("TestClasses");
    REQUIRE(0 == proj.types().size());

    TempFile temp_file;
    temp_file << R"SOURCE(
class AA {
public:
    class AB {};
};

class BB {
public:
    int a; // trap to make sure int is not a type of BB
};
)SOURCE";

    temp_file.close();
    proj.parse(temp_file.name);

    SECTION("has nested") {
        cwap::Type* aa_type = proj.types().at("AA");
        REQUIRE(aa_type->types().size() == 1);
    }
    SECTION("no nested, and int is not a nested class") {
        cwap::Type* bb_type = proj.types().at("BB");
        REQUIRE(bb_type->types().size() == 0);
        REQUIRE(proj.types().count("int") == 1);
        REQUIRE(bb_type->types().count("int") == 0);
    }
}
// TODO: protected methods, attributes
