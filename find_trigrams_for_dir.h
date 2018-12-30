#ifndef FIND_TRIGRAMS_FOR_DIR_H
#define FIND_TRIGRAMS_FOR_DIR_H

#include <QString>
#include <vector>
#include <QHash>

namespace my {
    extern QHash<QString, std::vector<int>> find_trigrams(QString dir, const bool &find_string_run);
}

#endif // FIND_TRIGRAMS_FOR_DIR_H
