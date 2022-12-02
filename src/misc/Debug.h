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

#ifndef YVM_INSPECTOR_H
#define YVM_INSPECTOR_H

#include <deque>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include "../runtime/JavaClass.h"

class JavaClass;

struct Inspector {
    static void printConstantPool(const JavaClass& jc);
    static void printClassAccessFlag(const JavaClass& jc);
    static void printField(const JavaClass& jc);
    static void printMethod(const JavaClass& jc);
    static void printClassFileVersion(const JavaClass& jc);
    static void printInterfaces(const JavaClass& jc);
    static void printClassFileAttrs(const JavaClass& jc);

    static void printSizeofInternalTypes();
    static void printOpcode(u1* code, u4 index);
};

class DbgPleasant {
public:
    DbgPleasant(std::string title, int columnCount)
        : title(std::move(title)), column(columnCount) {
        cells.emplace_back();
    }

    void setCellWidth(int w) { this->columnWidth = w; }

    void addCell(const std::string& str) {
        std::string nstr;
        if (str.length() > columnWidth - 2) {
            nstr = str.substr(0, columnWidth - 2);
        } else {
            nstr = str;
        }
        if (currentColumn == column) {
            cells.emplace_back();
            currentColumn = 0;
        }
        cells.at(cells.size() - 1).push_back(nstr);
        currentColumn++;
    }

    void show() {
        using namespace std;
        // Header
        for (int i = 0; i < columnWidth * column; i++) cout << "-";
        cout << "\n";
        unsigned long long int margin =
            ((column * columnWidth) - 2 - title.length()) / 2;
        cout << "/";
        for (int i = 0; i < margin; i++) cout << " ";
        cout << title;
        for (int i = 0; i < margin; i++) cout << " ";
        cout << "/\n";
        for (int i = 0; i < columnWidth * column; i++) cout << "-";
        cout << "\n";
        // Body
        for (auto& cell : cells) {
            for (auto& k : cell) {
                cout << "|";
                cout << k;
                for (int p = 0; (columnWidth - 2 - k.length()) > 0 &&
                                p < (columnWidth - 2 - k.length());
                     p++)
                    cout << " ";
                cout << "|";
            }
            cout << "\n";
        }
        // End
        for (int i = 0; i < columnWidth * column; i++) cout << "-";
        cout << "\n";
    }

private:
    std::string title;
    std::deque<std::deque<std::string>> cells;
    int currentColumn = 0;
    int column;
    int columnWidth = 30;
};

#endif  // YVM_INSPECTOR_H
