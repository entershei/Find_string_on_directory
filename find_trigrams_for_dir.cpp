#include "find_trigrams_for_dir.h"
#include "mainwindow.h"
#include <iostream>
#include <QDebug>

extern QPair<bool, QHash<QString, std::vector<int>> > my::find_trigrams(QString dir,
                                                          const std::atomic_bool &index_run) {
    long long n = 1000000000;
    QHash<QString, std::vector<int>> trigrams;

    qDebug() << "#my::find_trigrams";

    for (long long i = 0; i < n; ++i) {
        //qDebug() << i;
        if (!index_run) { qDebug() << "break index\n"; return {false, trigrams}; }
    }

    trigrams["a"].push_back(1);

    qDebug() << "#end find trigrams";

    return {true, trigrams};
}
