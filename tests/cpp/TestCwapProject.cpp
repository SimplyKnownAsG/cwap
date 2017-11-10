#include "cwap/cwap.hpp"

#include "catch.hpp"
#include "globpp.hpp"

TEST_CASE("parse own project", "[!mayfail]") {
    cwap::Project proj("cwap");
    auto paths = globpp::glob("src/cwap/*.hpp", true);
    std::vector<std::string> clang_args{ "-I./src/" };
    proj.parse(paths, clang_args);

    std::vector<std::string> patterns{ "src/**/*.hpp", "src/cwap/*.hpp" };
    for (auto pattern : patterns) {
        std::cout << pattern << std::endl;
        auto paths = globpp::glob(pattern, true);
        for (auto path : paths) {
            std::cout << "   " << path << std::endl;
        }
    }

    SECTION("project") {
        REQUIRE(1 == proj.namespaces().size());
        auto cwap = proj.namespaces().at("cwap");
        for (auto t : cwap->types()) {
            std::cout << t.first << std::endl;
        }
        REQUIRE(15 == cwap->types().size());
    }
}
