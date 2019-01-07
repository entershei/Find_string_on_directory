#include "find_trigrams_for_dir.h"
#include "mainwindow.h"
#include <iostream>
#include <QDebug>
#include <filesystem>
#include <QtConcurrent/QtConcurrentMap>
#include <fstream>
#include <unordered_set>
#include <algorithm>

namespace fs = std::filesystem;

//const size_t MAX_BLOCK = 1 << 12;
const size_t MAX_BLOCK = 10; //test for different blok with string
const size_t MAX_DIFFERENT_TRIGRAM = 200000;
bool bad_trigram(unsigned char a) {
    return (a == 0x00) || (a == 0xC0) || (a == 0xC1) || (0xF5 <= a && a <= 0xFF);
}

int get_trigram(char a, char b, char c, bool &has_bad_trigram) {
    if (bad_trigram(a) || bad_trigram(b) || bad_trigram(c)) {
        has_bad_trigram = true;
        return 0;
    }

    using uint = unsigned long;
    uint base = (1 << 8);

    return uint(a) * base * base + uint(b) * base + uint(c);
}

QHash<QString, std::vector<int>> mapTrigrams(const fs::directory_entry& dir_file) {
    QHash<QString, std::vector<int>> ret;

    std::ifstream file(dir_file.path());

    qDebug() << QString::fromUtf8(dir_file.path().c_str());

    if(file.is_open()) {
        bool not_utf8_file = false;
        std::unordered_set<int> different_trigrams;
        std::string buffer;
        buffer.resize(MAX_BLOCK);
        std::string previous_buffer = "";
        size_t size_buffer = 0;
        do{
            file.read(&buffer[previous_buffer.size()], MAX_BLOCK + previous_buffer.size());
            buffer.resize(size_t(file.gcount()) + previous_buffer.size());
            size_buffer = buffer.size() - previous_buffer.size();

            if (buffer.size() < 3) {
                break;
            }

            for (size_t i = 0; i < previous_buffer.size(); ++i) {
                buffer[i] = previous_buffer[i];
            }

            qDebug() << QString::fromUtf8(buffer.c_str()) << " " << buffer.size();

            for (size_t i = 0; i < buffer.size() - 2; ++i) {
                int cur_trigram = get_trigram(buffer[i], buffer[i + 1], buffer[i + 2], not_utf8_file);

                if (not_utf8_file) {
                    qDebug() << "not utf8";
                    break;
                } else {
                    different_trigrams.insert(cur_trigram);

                    if (different_trigrams.size() > MAX_DIFFERENT_TRIGRAM) {
                        not_utf8_file = true;
                        break;
                    }
                }
            }

            if (buffer.size() > 1) {
                previous_buffer = buffer.substr(buffer.size() - 2, 2);
            } else {
                previous_buffer = "";
            }

        } while(size_buffer > 0 && !not_utf8_file);

        std::vector<int> trigrams;

        if (!not_utf8_file) {
            for (auto i : different_trigrams) {
                trigrams.push_back(i);
            }
            std::sort(trigrams.begin(), trigrams.end());

            ret.insert(QString::fromUtf8(dir_file.path().c_str()), trigrams);

            qDebug() << "add to files_trigrams " <<QString::fromUtf8(dir_file.path().c_str());

            /*for (auto i : trigrams) {
                qDebug() << i << " " <<  char(i / 256 / 256) << char((i / 256) % 256) << char(i % 256);
            }*/
        }
    }
    return ret;
}

struct MappingFunctor {
    MappingFunctor(const std::atomic_bool &index_run): index_run(index_run) {}

    QHash<QString, std::vector<int>> operator()(const fs::directory_entry& file) const {
        if (index_run) {
            return mapTrigrams(file);
        } else {
            return QHash<QString, std::vector<int>>();
        }
    }

    using result_type = QHash<QString, std::vector<int>>;
    const std::atomic_bool &index_run;
};

void reduceTrigrams(QHash<QString, std::vector<int>> &l_hash, const QHash<QString, std::vector<int>> &r_hash) {
    l_hash.unite(r_hash);
}

extern QPair<bool, QHash<QString, std::vector<int>> > my::find_trigrams(QString dir,
                                                          const std::atomic_bool &index_run) {
    QHash<QString, std::vector<int>> all_trigrams;
    fs::path new_path(dir.toStdString());

    std::vector<fs::directory_entry> files;

    for (auto& cur_file: fs::recursive_directory_iterator(new_path, fs::directory_options::skip_permission_denied)) {
        if (!index_run) { return {false, all_trigrams}; }

        files.push_back(cur_file);
    }

    if (!index_run) { return {false, all_trigrams}; }

    all_trigrams = QtConcurrent::blockingMappedReduced(files, MappingFunctor(index_run), reduceTrigrams);

    if (index_run) {
        return {true, all_trigrams};
    } else {
        return {false, all_trigrams};
    }
}
