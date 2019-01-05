#include "find_trigrams_for_dir.h"
#include "mainwindow.h"
#include <iostream>
#include <QDebug>
#include <filesystem>
#include <QtConcurrent/QtConcurrent>

namespace fs = std::filesystem;

std::vector<int> find_trigrams_on_file(fs::directory_entry file) {

}

extern QPair<bool, QHash<QString, std::vector<int>> > my::find_trigrams(QString dir,
                                                          const std::atomic_bool &index_run) {
    QHash<QString, std::vector<int>> trigrams;

    fs::path new_path(dir.toStdString());

    std::vector<fs::directory_entry> files;

    for(auto& cur_file: fs::recursive_directory_iterator(new_path, fs::directory_options::skip_permission_denied)) {
        if (!index_run) { return {false, trigrams}; }

        files.push_back(cur_file);
    }

    if (!index_run) { return {false, trigrams}; }

    QtConcurrent::blockingMap(files, [&trigrams] (auto &&file) { trigrams.insert(file.path, find_trigrams_on_file(file)); });

    return {true, trigrams};
}
