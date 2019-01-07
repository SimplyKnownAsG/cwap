#include "TempFile.hpp"

#include "cwap/cwap.hpp"

#include "catch.hpp"

#include <algorithm>
#include <functional>
#include <sstream>

using namespace std;

TEST_CASE("functions", "[functions]") {
    cwap::Project proj("TestFunctions");
    REQUIRE(0 == proj.types().size());
    TempFile temp_file;

    string expected_blemish = "";

    SECTION("void (void)") {
        temp_file << "void first() { return; } ;" << endl;
        temp_file.close();
        proj.parse(temp_file.name);

        cwap::Function& func = *proj.functions()[0];
        REQUIRE("first" == func.name);
        REQUIRE(proj.types().at("void") == func.return_type);
        REQUIRE(func.parameters().size() == 0);

        expected_blemish = "void first();";
    }
    SECTION("void (int)") {
        temp_file << "void func(int a) { return; } ;" << endl;
        temp_file.close();
        proj.parse(temp_file.name);

        cwap::Function& func = *proj.functions()[0];
        REQUIRE(proj.types().at("void") == func.return_type);
        REQUIRE(func.parameters().size() == 1);
        auto param_a = func.parameters()[0];
        REQUIRE(param_a->name == "a");

        expected_blemish = "void func(int a);";
    }
    SECTION("int (void)") {
        temp_file << "int first() { return 1; }" << endl;
        temp_file.close();
        proj.parse(temp_file.name);

        cwap::Function& func = *proj.functions()[0];
        REQUIRE("first" == func.name);
        REQUIRE(proj.types().at("int") == func.return_type);
        REQUIRE(func.parameters().size() == 0);

        expected_blemish = "int first();";
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

        expected_blemish = "int def_decl();";
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

        cwap::TypeUsage* param = proj.functions()[0]->parameters()[0];
        REQUIRE(param->name == "param1");

        expected_blemish = "int def_decl(int param1);";
    }

    ostringstream actual_blemish;
    proj.write_header(actual_blemish);

    REQUIRE_THAT(actual_blemish.str(), Catch::Contains(expected_blemish));
}

TEST_CASE("overloaded functions", "[functions][overloaded]") {
    cwap::Project proj("TestFunctions");

    TempFile temp_file;
    temp_file << R"SOURCE(
int overloaded();
int overloaded(int param1);
int overloaded(float floaty);
)SOURCE";

    temp_file.close();
    proj.parse(temp_file.name);
    auto functions = proj.functions();
    REQUIRE(functions.size() == 3);

    for (auto func : functions) {
        REQUIRE("overloaded" == func->name);
    }

    function<bool(cwap::Function*)> predicate;

    SECTION("first overload without parameter") {
        predicate = [](cwap::Function* meth) -> bool { return meth->parameters().size() == 0; };
    }
    SECTION("second overload with int parameter") {
        predicate = [](cwap::Function* meth) -> bool {
            return meth->parameters().size() == 1 && meth->parameters()[0]->name == "param1";
        };
        auto matches = find_if(functions.begin(), functions.end(), predicate);
        cwap::Function* func = *matches;
        REQUIRE(func->parameters().size() == 1);
        cwap::TypeUsage* param = func->parameters()[0];
        REQUIRE(param->name == "param1");
        REQUIRE(param->cwap_type == proj.types().at("int"));
    }
    SECTION("third overload with float parameter") {
        predicate = [](cwap::Function* meth) -> bool {
            return meth->parameters().size() == 1 && meth->parameters()[0]->name == "floaty";
        };
        auto matches = find_if(functions.begin(), functions.end(), predicate);
        cwap::Function* func = *matches;
        REQUIRE(func->parameters().size() == 1);
        cwap::TypeUsage* param = func->parameters()[0];
        REQUIRE(param->name == "floaty");
        REQUIRE(param->cwap_type == proj.types().at("float"));
    }
    REQUIRE(1 == count_if(functions.begin(), functions.end(), predicate));
}

TEST_CASE("template functions", "[functions]") {
    cwap::Project proj("TestFunctions");
    REQUIRE(0 == proj.types().size());
    TempFile temp_file;

    string expected_blemish = "";

    SECTION("template parameter") {
        expected_blemish = "template <typename T> T function(T val)\n"
                           "{ return val; };";
        temp_file << expected_blemish;
        temp_file.close();
        proj.parse(temp_file.name);

        cwap::Function& func = *proj.functions()[0];
        REQUIRE(func.parameters().size() == 1);
    }

    ostringstream actual_blemish;
    proj.write_header(actual_blemish);

    REQUIRE_THAT(actual_blemish.str(), Catch::Contains(expected_blemish));
}
