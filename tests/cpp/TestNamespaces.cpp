#include "TempFile.hpp"

#include "cwap/cwap.hpp"

#include "catch.hpp"

TEST_CASE("namespaces with builtin types", "[namespaces]") {
    cwap::Project proj("TestFunctions");
    REQUIRE(0 == proj.types().size());
    TempFile temp_file;

    SECTION("int in two namespaces") {
        temp_file << R"SOURCE(
int a;
namespace b {
    int a;
}
)SOURCE";
        temp_file.close();
        proj.parse(temp_file.name);

        REQUIRE(1 == proj.types().size());

        cwap::Namespace* space = proj.namespaces().at("b");
        REQUIRE(0 == space->types().size());
        REQUIRE("a" == space->variables().at("a")->name);
    }
    SECTION("namespace declared twice") {
        temp_file << R"SOURCE(
namespace a {
    int b;
}
namespace a {
    int c;
}
)SOURCE";
        temp_file.close();
        proj.parse(temp_file.name);

        REQUIRE(1 == proj.types().size());
        REQUIRE(1 == proj.namespaces().size());
        REQUIRE(0 == proj.variables().size());

        cwap::Namespace* space = proj.namespaces().at("a");
        REQUIRE(0 == space->types().size());
        REQUIRE(2 == space->variables().size());
        REQUIRE("b" == space->variables().at("b")->name);
        REQUIRE("c" == space->variables().at("c")->name);
    }
    SECTION("classes with the same name in different namespaces") {
        temp_file << R"SOURCE(
class A {};
namespace a {
    class A {};
}
namespace b {
    class A {};
}
)SOURCE";
        temp_file.close();
        proj.parse(temp_file.name);

        REQUIRE(1 == proj.types().size());
        REQUIRE(2 == proj.namespaces().size());
        REQUIRE(0 == proj.variables().size());

        cwap::Namespace* space_a = proj.namespaces().at("a");
        cwap::Namespace* space_b = proj.namespaces().at("b");
        REQUIRE(space_a != space_b);
        REQUIRE(1 == space_a->types().size());
        REQUIRE(1 == space_b->types().size());
        cwap::Type* proj_A = proj.types().at("A");
        cwap::Type* a_A = space_a->types().at("a::A");
        cwap::Type* b_A = space_b->types().at("b::A");
        REQUIRE(proj_A != a_A);
        REQUIRE(a_A != b_A);
    }
}

TEST_CASE("std namespace", "[namespaces]") {
    cwap::Project proj("TestFunctions");
    REQUIRE(0 == proj.types().size());
    TempFile temp_file;

    SECTION("std namespace being read does not add to project") {
        temp_file << R"SOURCE(
#include <string>
int a;
)SOURCE";
        temp_file.close();
        proj.parse(temp_file.name);

        REQUIRE(1 == proj.types().size());
        REQUIRE(0 == proj.namespaces().size());
    }
    SECTION("using std namespace does not add to project") {
        temp_file << R"SOURCE(
#include <string>
using namespace std;
int a;
)SOURCE";
        temp_file.close();
        proj.parse(temp_file.name);

        REQUIRE(1 == proj.types().size());
        REQUIRE(0 == proj.namespaces().size());
    }
    SECTION("public use of std namespace adds it to project") {
        temp_file << R"SOURCE(
#include <string>
using namespace std;
int a;
string b;
)SOURCE";
        temp_file.close();
        proj.parse(temp_file.name);

        REQUIRE(1 == proj.namespaces().size());
        REQUIRE(1 == proj.types().size());
        auto std_space = proj.namespaces().at("std");
        // std::string is really std::__cxx11::string or something like that
        REQUIRE(1 == std_space->namespaces().begin()->second->types().size());
        REQUIRE(1 == std_space->namespaces().begin()->second->types().count("std::string"));
    }
    SECTION("public use of std::string adds std to project") {
        temp_file << R"SOURCE(
#include <string>
int a;
std::string b;
)SOURCE";
        temp_file.close();
        proj.parse(temp_file.name);

        REQUIRE(1 == proj.namespaces().size());
        REQUIRE(1 == proj.types().size());
        auto std_space = proj.namespaces().at("std");
        // std::string is really std::__cxx11::string or something like that
        REQUIRE(1 == std_space->namespaces().begin()->second->types().size());
        REQUIRE(1 == std_space->namespaces().begin()->second->types().count("std::string"));
    }
}
// TODO: namespace alias
