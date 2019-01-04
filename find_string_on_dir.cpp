#include "find_string_on_dir.h"

#include <iostream>
#include <QDebug>

extern QPair<bool, QVector<QString>> my_find_string::find_string(QString const &dir, QString const &stringForSearch,
                                                    const std::atomic_bool &find_string_run) {
    qDebug() << "%my_find_string::find_string";
    long long n = 100000000;

    QVector<QString> files;

    for (long long i = 0; i < n; ++i) {
       if (!find_string_run) { qDebug() << "break fing string\n"; return {false, files}; }
    }

    files.push_back("abc.txt");

    qDebug() << "%end find string" << files.size();

    return {true, files};
}
