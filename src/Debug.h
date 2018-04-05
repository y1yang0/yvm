#ifndef YVM_INSPECTOR_H
#define YVM_INSPECTOR_H

#include "JavaClass.h"
#include <deque>
#include <iostream>
#include <string>
#include <iomanip>
#include <utility>

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
    DbgPleasant(std::string title, int columnCount) : title(std::move(title)), column(columnCount) {
        cells.emplace_back();
    }

    void setCellWidth(int w) {
        this->columnWidth = w;
    }

    void addCell(const std::string& str) {
        std::string nstr;
        if (str.length() > columnWidth - 2) {
            nstr = str.substr(0, columnWidth - 2);
        }
        else {
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
        //Header
        for (int i = 0; i < columnWidth * column; i++) cout << "-";
        cout << "\n";
        unsigned long long int margin = ((column * columnWidth) - 2 - title.length()) / 2;
        cout << "/";
        for (int i = 0; i < margin; i++)cout << " ";
        cout << title;
        for (int i = 0; i < margin; i++)cout << " ";
        cout << "/\n";
        for (int i = 0; i < columnWidth * column; i++) cout << "-";
        cout << "\n";
        //Body
        for (auto& cell : cells) {
            for (auto& k : cell) {
                cout << "|";
                cout << k;
                for (int p = 0; (columnWidth - 2 - k.length()) > 0 && p < (columnWidth - 2 - k.length()); p++)
                    cout <<
                        " ";
                cout << "|";
            }
            cout << "\n";

        }
        //End
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

#endif //YVM_INSPECTOR_H
