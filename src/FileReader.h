#ifndef YVM_FILEREADER_H
#define YVM_FILEREADER_H

#include <fstream>
#include "Internal.h"

class FileReader {
public:
    FileReader() = default;

    // We must set file stream mode as std::ios::binary instead of default text
    // reading mode
    FileReader(const std::string& filePath) : fin(filePath, std::ios::binary) {
        this->filePath = filePath;
    }

    ~FileReader() { fin.close(); }

    bool openFile(std::string filePath) {
        if (!fin.is_open()) {
            fin.open(filePath, std::ios::binary);
            return fin.is_open();
        }
        return true;
    }

    bool haveNoExtraBytes() { return fin.peek() == EOF; }

    u4 readget4() {
        fin.read(u4buf, 4);
        return getu4(u4buf);
    }

    u2 readget2() {
        fin.read(u2buf, 2);
        return getu2(u2buf);
    }

    u1 readget1() {
        fin.read(u1buf, 1);
        return getu1(u1buf);
    };

private:
    std::ifstream fin;
    std::string filePath;
    char u4buf[4];
    char u2buf[2];
    char u1buf[1];
};

#endif  // !YVM_FILEREADER_H
