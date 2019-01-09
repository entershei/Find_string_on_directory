#include "find_string_on_dir.h"

#include <iostream>
#include <QDebug>
#include <fstream>
#include <QtConcurrent/QtConcurrentMap>
#include <functional>
#include <optional>

typedef std::boyer_moore_searcher<__gnu_cxx::__normal_iterator<char *,
            std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >,
            std::hash<char>, std::equal_to<void> > boyer_moore_searcher_t;

int get_trigrams(char a, char b, char c) {
    int base = (1 << 8);
    return int(a) * base * base + int(b) * base + int(c);
}

bool has_trigrams(const std::vector<int> &file_trigrams, int trigram) {
     return (std::lower_bound(file_trigrams.begin(), file_trigrams.end(), trigram)) != file_trigrams.end();
}

bool string_searcher(const QString &file_path, size_t size_string_for_search,
                     const std::atomic_bool &find_string_run,
                     const boyer_moore_searcher_t &b_m_searcher,
                     const std::string &string_for_search) {
    if (!find_string_run) { return  false; }

    std::ifstream file(file_path.toStdString());

    int MAX_BLOCK = int(std::max(size_t(1 << 12), size_string_for_search + 1));
    //int MAX_BLOCK = int(std::max(size_t(10), size_string_for_search + 1)); //test for different blok with string in smalltestwith_aaabc

    if (!find_string_run) { return  false; }

    if(file.is_open()) {
        std::string buffer;
        buffer.resize(size_t(MAX_BLOCK) + size_string_for_search + 1);
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
            //auto it = std::search(buffer.begin(), buffer.end(), std::boyer_moore_searcher(string_for_search.begin(), string_for_search.end()));
            //auto it = std::search(buffer.begin(), buffer.end(), string_for_search.begin(), string_for_search.end());

            auto it = std::search(buffer.begin(), buffer.end(), b_m_searcher);

            if (it != buffer.end()) {
                return true;
            }

            if (!find_string_run) { return  false; }

            if (size_buffer > size_string_for_search) {
                previous_buffer = buffer.substr(size_buffer - size_string_for_search + 1,
                                                size_t(int(size_string_for_search) - 1));
            } else {
                previous_buffer = "";
            }
        } while(size_new_buffer > 0);
    }

    return false;
}

struct MappingFunctor {
    MappingFunctor(const QHash<QString, std::vector<int>> &files_trigrams, const QString &QString_for_search,
                  const std::atomic_bool &find_string_run): files_trigrams(files_trigrams),
                                                            find_string_run(find_string_run) {
        string_for_search = QString_for_search.toStdString();
        b_m_searcher = std::boyer_moore_searcher(string_for_search.begin(), string_for_search.end());
    }

    QVector<QString> operator() (const QString& name_file) const {
        if (!find_string_run) { return {}; }
        //qDebug() << "check file: " << name_file;

        bool has_all_trigrams_of_string = true;
        for (int i = 0; i < int(string_for_search.size()) - 2; ++i) {
            if (!find_string_run) { return {}; }

            int cur_trigrams = get_trigrams(string_for_search[size_t(i)], string_for_search[size_t(i + 1)],
                                            string_for_search[size_t(i + 2)]);

            if (!has_trigrams(files_trigrams.value(name_file), cur_trigrams)) {
                has_all_trigrams_of_string = false;
                break;
            }
        }

        if (!find_string_run) { return {}; }

        if (has_all_trigrams_of_string) {
            if (string_searcher(name_file, string_for_search.size(), find_string_run, b_m_searcher.value(), string_for_search)) {
                return {name_file};
            }
        }
        return {};
    }

    using result_type = QVector<QString>;

    const QHash<QString, std::vector<int>> &files_trigrams;
    std::string string_for_search;
    const std::atomic_bool &find_string_run;
    std::optional<boyer_moore_searcher_t> b_m_searcher;
};

struct ReducingFunctor {
    ReducingFunctor(const std::atomic_bool &find_string_run): find_string_run(find_string_run) {}

    void operator() (QVector<QString> &l_vector, const QVector<QString> &r_vector) {
        if (find_string_run) {
            l_vector.append(r_vector);
        } else {
            return;
        }
    }

    const std::atomic_bool &find_string_run;
};

extern QPair<bool, QVector<QString>> my_find_string::find_string(QString const &QString_for_search,
                                                    const QHash<QString, std::vector<int>> &files_trigrams,
                                                    const std::atomic_bool &find_string_run) {

    if (!find_string_run) { return {false, {}}; }

    QVector<QString> file_with_string;

    auto mpf = MappingFunctor(files_trigrams, QString_for_search, find_string_run);
    auto rdf = ReducingFunctor(find_string_run);
    file_with_string = QtConcurrent::blockingMappedReduced<QVector<QString>>(files_trigrams.keys(), mpf, rdf);

    if (!find_string_run) { return {false, {}}; }
    return {true, file_with_string};
}
