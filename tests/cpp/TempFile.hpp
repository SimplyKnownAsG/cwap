#include <cstdio>
#include <fstream>
#include <string>

class TempFile {
private:
    std::ofstream* _stream;

    template<typename T>
    friend std::ofstream& operator<<(TempFile&, T);

public:
    const std::string name;

    TempFile(std::string name);

    TempFile();

    void close();

    ~TempFile();
};

template<typename T>
std::ofstream& operator<<(TempFile& temp, T op) {
    (*temp._stream) << op;
    return *temp._stream;
}
