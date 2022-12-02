// MIT License
//
// Copyright (c) 2017 Yi Yang <kelthuzadx@qq.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifndef YVM_FILEREADER_H
#define YVM_FILEREADER_H

#include <fstream>
#include "../interpreter/Internal.h"

// get big-endian integer representation
#define getu4(buf)                                                           \
    (((*(u4*)buf) & 0x000000FF) << 24) | (((*(u4*)buf) & 0x0000FF00) << 8) | \
        (((*(u4*)buf) & 0x00FF0000) >> 8) | (((*(u4*)buf) & 0xFF000000) >> 24)

#define getu2(buf) \
    (u2)((((*(u2*)buf) & 0x00FF) << 8) | (((*(u2*)buf) & 0xFF00) >> 8))

#define getu1(buf) *(u1*)buf

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
