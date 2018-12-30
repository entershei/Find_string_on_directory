#include "find_trigrams_for_dir.h"
#include "mainwindow.h"
#include <iostream>

extern QHash<QString, std::vector<int>> my::find_trigrams(QString dir, const bool &index_run) {
    long long n = 1000000000000;

    QHash<QString, std::vector<int>> trigrams;

    for (int i = 0; i < n; ++i) {
       if (!index_run) { std::cerr << "break index\n"; return trigrams; }
       //std::cerr << i << "\n";
    }

    trigrams["a"].push_back(1);

    qDebug() << "end find trigrams";

    return trigrams;
}
