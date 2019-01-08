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

const size_t MAX_BLOCK = 1 << 12;
//const size_t MAX_BLOCK = 10; //test for different blok with string
const size_t MAX_DIFFERENT_TRIGRAM = 200000;
bool bad_trigram(char a) {
    return (a == char(0x00)) || (a == char(0xC0)) || (a == char(0xC1)) || (char(0xF5) <= a && a <= char(0xFF));
}

int get_trigram(char a, char b, char c, bool &has_bad_trigram) {
    if (bad_trigram(a) || bad_trigram(b) || bad_trigram(c)) {
        has_bad_trigram = true;
        return 0;
    }

    int base = (1 << 8);
    return int(a) * base * base + int(b) * base + int(c);
}

QHash<QString, std::vector<int>> mapTrigrams(const fs::directory_entry& dir_file, const std::atomic_bool & index_run) {
    QHash<QString, std::vector<int>> ret;

    if (!index_run) { return ret; }

    std::ifstream file(dir_file.path());

    if(file.is_open()) {        
        if (!index_run) { return ret; }

        bool not_utf8_file = false;
        std::unordered_set<int> different_trigrams;
        std::string buffer;
        buffer.resize(MAX_BLOCK + 3);
        std::string previous_buffer = "";
        size_t size_buffer = 0;
        size_t size_new_buffer = 0;

        if (!index_run) { return ret; }

        do{            
            if (!index_run) { return ret; }

            file.read(&buffer[previous_buffer.size()], MAX_BLOCK);
            size_new_buffer = size_t(file.gcount());
            size_buffer = size_new_buffer + previous_buffer.size();

            if (size_buffer < 3) {
                break;
            }

            for (size_t i = 0; i < previous_buffer.size(); ++i) {
                buffer[i] = previous_buffer[i];
            }

            for (size_t i = 0; i < size_buffer - 2; ++i) {
                if (!index_run) { return ret; }

                int cur_trigram = get_trigram(buffer[i], buffer[i + 1], buffer[i + 2], not_utf8_file);

                if (not_utf8_file) {
                    qDebug() << "not utf8 " << QString::fromUtf8(dir_file.path().c_str()) << " " << i << " " <<  buffer[i] << buffer[i + 1] << buffer[i + 2];
                    break;
                } else {                    
                    if (!index_run) { return ret; }

                    different_trigrams.insert(cur_trigram);

                    if (different_trigrams.size() > MAX_DIFFERENT_TRIGRAM) {
                        not_utf8_file = true;
                        break;
                    }
                }
            }

            if (size_buffer > 1) {
                if (!index_run) { return ret; }

                previous_buffer = buffer.substr(size_buffer - 2, 2);
            } else {
                previous_buffer = "";
            }

        } while(size_new_buffer > 0 && !not_utf8_file);

        if (!index_run) { return ret; }

        std::vector<int> trigrams;

        if (!not_utf8_file) {
            for (auto i : different_trigrams) {                
                if (!index_run) { return ret; }

                trigrams.push_back(i);
            }
            std::sort(trigrams.begin(), trigrams.end());

            ret.insert(QString::fromUtf8(dir_file.path().c_str()), trigrams);

            qDebug() << "add to files_trigrams " <<QString::fromUtf8(dir_file.path().c_str());
        }
    }
    return ret;
}

struct MappingFunctor {
    MappingFunctor(const std::atomic_bool &index_run): index_run(index_run) {}

    QHash<QString, std::vector<int>> operator()(const fs::directory_entry& file) const {
        if (index_run) {
            return mapTrigrams(file, index_run);
        } else {
            return QHash<QString, std::vector<int>>();
        }
    }

    using result_type = QHash<QString, std::vector<int>>;
    const std::atomic_bool &index_run;
};

struct ReducingFunctor {
    ReducingFunctor(const std::atomic_bool &index_run): index_run(index_run) {}

    void operator() (QHash<QString, std::vector<int>> &l_hash, const QHash<QString, std::vector<int>> &r_hash) {
        if (index_run) {
            l_hash.unite(r_hash);
        } else {
            return;
        }
    }
    const std::atomic_bool &index_run;
};

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

    all_trigrams = QtConcurrent::blockingMappedReduced<QHash<QString, std::vector<int>> >(files, MappingFunctor(index_run),
                                                       ReducingFunctor(index_run));

    if (index_run) {
        return {true, all_trigrams};
    } else {
        return {false, all_trigrams};
    }
}
