#ifndef FIND_STRING_ON_DIR_H
#define FIND_STRING_ON_DIR_H

#include <QVector>
#include <QString>
#include <atomic>
#include <QPair>

namespace my_find_string {
    extern QPair<bool, QVector<QString>> find_string(QString const &dir, QString const &stringForSearch,
                                        const std::atomic_bool &find_string_run);
}

#endif // FIND_STRING_ON_DIR_H
