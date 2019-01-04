#ifndef FIND_TRIGRAMS_FOR_DIR_H
#define FIND_TRIGRAMS_FOR_DIR_H

#include <QString>
#include <vector>
#include <QHash>
#include <atomic>

namespace my {
    extern QPair<bool, QHash<QString, std::vector<int>> > find_trigrams(QString dir, const std::atomic_bool &find_string_run);
}

#endif // FIND_TRIGRAMS_FOR_DIR_H
