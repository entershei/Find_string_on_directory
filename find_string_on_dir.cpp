#include "find_string_on_dir.h"

#include <iostream>

extern QVector<QString> my_find_string::find_string(QString const &dir, QString const &stringForSearch, const bool &find_string_run) {
    long long n = 1000000000000;

    QVector<QString> files;

    for (int i = 0; i < n; ++i) {
       if (!find_string_run) { std::cerr << "break fing string\n"; return files; }
        //std::cerr << i << "!";
    }

    std::cerr << "end find string\n";
    files.push_back("abc.txt");

    return files;
}
