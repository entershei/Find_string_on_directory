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

bool string_searcher(const QString &file_path, const std::string &string_for_search, const std::atomic_bool &find_string_run) {
    if (!find_string_run) { return  false; }

    std::ifstream file(file_path.toStdString());

    int MAX_BLOCK = int(std::max(size_t(1 << 12), string_for_search.size() + 1));
    //int MAX_BLOCK = int(std::max(size_t(10), string_for_search.size() + 1)); //test for different blok with string in smalltestwith_aaabc

    if (!find_string_run) { return  false; }

    if(file.is_open()) {
        std::string buffer;
        buffer.resize(size_t(MAX_BLOCK) + size_t(string_for_search.size()) + 1);
        std::string previous_buffer = "";
        size_t size_buffer = 0;
        size_t size_new_buffer = 0;
        do{ 
            if (!find_string_run) { return  false; }

            file.read(&buffer[previous_buffer.size()], MAX_BLOCK);
            size_new_buffer = size_t(file.gcount());
            size_buffer = size_new_buffer + previous_buffer.size();

            for (size_t i = 0; i < previous_buffer.size(); ++i) {          
                if (!find_string_run) { return  false; }

                buffer[i] = previous_buffer[i];
            }

            auto it = std::search(buffer.begin(), buffer.end(), string_for_search.begin(), string_for_search.end());
            if (it != buffer.end()) {
                return true;
            }

            if (!find_string_run) { return  false; }

            if (size_buffer > size_t(string_for_search.size())) {
                previous_buffer = buffer.substr(size_buffer - size_t(string_for_search.size()) + 1,
                                                size_t(string_for_search.size() - 1));
            } else {
                previous_buffer = "";
            }
        } while(size_new_buffer > 0);
    }

    return false;
}

extern QPair<bool, QVector<QString>> my_find_string::find_string(QString const &QString_for_search,
                                                    const QHash<QString, std::vector<int>> &files_trigrams,
                                                    const std::atomic_bool &find_string_run) {

    if (!find_string_run) { return {false, {}}; }

    QVector<QString> file_with_string;

    std::string string_for_search = QString_for_search.toStdString();

    for (auto& i : files_trigrams.keys()) {
        if (!find_string_run) { return {false, {}}; }

        bool has_all_trigrams_of_string = true;
        for (int j = 0; j < int(string_for_search.size()) - 3; ++j) {
            if (!find_string_run) { return {false, {}}; }

            int cur_trigrams = get_trigrams(string_for_search[size_t(j)], string_for_search[size_t(j + 1)],
                                            string_for_search[size_t(j + 2)]);

            if (!has_trigrams(files_trigrams.value(i), cur_trigrams)) {
                has_all_trigrams_of_string = false;
                break;
            }
        }

        if (!find_string_run) { return {false, {}}; }

        if (has_all_trigrams_of_string) {
            if (string_searcher(i, string_for_search, find_string_run)) {
                file_with_string.append(i);
            }
        }
    }
    if (!find_string_run) { return {false, {}}; }
    return {true, file_with_string};
}
