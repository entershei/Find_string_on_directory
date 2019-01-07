#include "find_string_on_dir.h"

#include <iostream>
#include <QDebug>
#include <fstream>

int get_trigrams(char a, char b, char c) {
    int base = (1 << 8);
    return int(a) * base * base + int(b) * base + int(c);
}

bool has_trigrams(const std::vector<int> &file_trigrams, int trigram) {
     return (std::lower_bound(file_trigrams.begin(), file_trigrams.end(), trigram)) != file_trigrams.end();
}

bool boyer_moore_searcher(const QString &file_path, const QString &stringForSearch) {
    std::ifstream file(file_path.toStdString());
    size_t MAX_BLOCK = std::max((1 << 12), stringForSearch.size() + 1);
    //int MAX_BLOCK = std::max(10, stringForSearch.size() + 1); //test for different blok with string in smalltestwith_aaabc

    if(file.is_open()) {
        std::string buffer;
        buffer.resize(size_t(MAX_BLOCK) + size_t(stringForSearch.size()) + 1);
        std::string previous_buffer = "";
        size_t size_buffer = 0;
        size_t size_new_buffer = 0;
        do{ 
            file.read(&buffer[previous_buffer.size()], MAX_BLOCK);
            size_new_buffer = size_t(file.gcount());
            size_buffer = size_new_buffer + previous_buffer.size();

            for (size_t i = 0; i < previous_buffer.size(); ++i) {
                buffer[i] = previous_buffer[i];
            }

            auto it = std::search(buffer.begin(), buffer.end(), stringForSearch.begin(), stringForSearch.end());
            if (it != buffer.end()) {
                return true;
            }

            if (size_buffer > size_t(stringForSearch.size())) {
                previous_buffer = buffer.substr(size_buffer - size_t(stringForSearch.size()) + 1,
                                                size_t(stringForSearch.size() - 1));
            } else {
                previous_buffer = "";
            }
        } while(size_new_buffer > 0);
    }

    return false;
}

extern QPair<bool, QVector<QString>> my_find_string::find_string(QString const &stringForSearch,
                                                    const QHash<QString, std::vector<int>> &files_trigrams,
                                                    const std::atomic_bool &find_string_run) {


    if (!find_string_run) { return {false, {}}; }

    QVector<QString> file_with_string;

    for (auto& i : files_trigrams.keys()) {
        bool has_all_trigrams_of_string = true;
        for (int j = 0; j < stringForSearch.size() - 3; ++j) {
            int cur_trigrams = get_trigrams(stringForSearch.at(j).toLatin1(), stringForSearch.at(j + 1).toLatin1(),
                                            stringForSearch.at(j + 2).toLatin1());

            if (!has_trigrams(files_trigrams.value(i), cur_trigrams)) {
                has_all_trigrams_of_string = false;
                break;
            }
        }

        if (has_all_trigrams_of_string) {
            if (boyer_moore_searcher(i, stringForSearch)) {
                file_with_string.append(i);
            }
        }
    }

    return {true, file_with_string};
}
