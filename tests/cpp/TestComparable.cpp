#include "TempFile.hpp"

#include "cwap/cwap.hpp"
#include "cwap/interfaces/Comparable.hpp"

#include "catch.hpp"

#include <algorithm>
#include <functional>

using cwap::Project;
using cwap::Type;
using cwap::interfaces::Comparable;

TEST_CASE("comparable", "[operators]") {
    Project proj("TestClasses");
    TempFile temp_file;

    temp_file << "class A {" << std::endl;

    auto do_assert = [&](bool success) -> void {
        temp_file << "};\n";
        temp_file.close();
        proj.parse(temp_file.name);

        Type* type = proj.types().at("A");
        auto c = Comparable(*type);
        REQUIRE(c.implemented() == success);
    };

    SECTION("is comparable") {
        temp_file << "public: bool operator<(A);\n";
        temp_file << "public: bool operator==(A);\n";
        temp_file << "public: bool operator>(A);\n";
        do_assert(true);
    }

    SECTION("bad return type") {
        temp_file << "public: bool operator<(A);\n";
        temp_file << "public: float operator==(A);\n";
        temp_file << "public: bool operator>(A);\n";
        do_assert(false);
    }

    SECTION("missing lt") {
        temp_file << "public: bool operator==(A);\n";
        temp_file << "public: bool operator>(A);\n";
        do_assert(false);
    }

    SECTION("missing eq") {
        temp_file << "public: bool operator<(A);\n";
        temp_file << "public: bool operator>(A);\n";
        do_assert(false);
    }

    SECTION("missing gt") {
        temp_file << "public: bool operator<(A);\n";
        temp_file << "public: bool operator==(A);\n";
        do_assert(false);
    }
}

