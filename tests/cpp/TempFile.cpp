#include "TempFile.hpp"

#include "catch.hpp"

TempFile::TempFile(std::string name)
  : name(name) {
    this->_stream = new std::ofstream(name, std::ofstream::trunc);
}

TempFile::TempFile()
  : TempFile(Catch::getResultCapture().getCurrentTestName() + ".cpp") {}

void TempFile::close() {
    this->_stream->close();
}

TempFile::~TempFile() {
    delete this->_stream;
    std::remove(this->name.c_str());
}
