#include "TempFile.hpp"

#include "cwap/cwap.hpp"

#include "catch.hpp"

#include <algorithm>
#include <functional>

TEST_CASE("inheritance", "[public header]") {
    cwap::Project proj("TestClasses");
    TempFile temp_file;
    temp_file << R"SOURCE(
class A { };
class B { };
class C { };

class D : public A, private B, protected C {};
)SOURCE";

    temp_file.close();
    proj.parse(temp_file.name);
    proj.write_header(std::cout);

    auto d = proj.types().at("D");
}

TEST_CASE("nested classses header", "[public header]") {
    cwap::Project proj("TestClasses");
    REQUIRE(0 == proj.types().size());

    TempFile temp_file;
    temp_file << R"SOURCE(
#include <vector>
using std::vector;
class Unspaced {};
namespace z {
    namespace z2 {
        class AA {
        public:
            class AB {};
            AB method();
        private:
            int p;
            unsigned char bitfield0 : 3;
            unsigned char bitfield1 : 1;
            bool someswitch;
            vector<int> something;
        };

        class BB {
        public:
            int a; // trap to make sure int is not a type of BB
            AA* method();
        };
    }
}
)SOURCE";

    temp_file.close();
    proj.parse(temp_file.name);
    proj.write_header(std::cout);
}
// TODO: protected methods, attributes
