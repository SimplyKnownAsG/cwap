#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <cstdio>
#include <fstream>
#include <string>

class TempFile {
private:
    std::ofstream* _stream;

    template <typename T>
    friend std::ofstream& operator<<(TempFile&, T);

public:
    const std::string name;

    TempFile(std::string name)
      : name(name) {
        this->_stream = new std::ofstream(name);
    }

    TempFile()
      : TempFile(Catch::getResultCapture().getCurrentTestName() + ".cpp"){};

    void close() {
        this->_stream->close();
    };

    ~TempFile() {
        delete this->_stream;
        std::remove(this->name.c_str());
    };
};

template <typename T>
std::ofstream& operator<<(TempFile& temp, T op) {
    (*temp._stream) << op;
    return *temp._stream;
}
