#ifndef FIND_STRING_ON_DIR_H
#define FIND_STRING_ON_DIR_H

#include <QVector>
#include <QString>

namespace my_find_string {
    extern QVector<QString> find_string(QString const &dir, QString const &stringForSearch, const bool &find_string);
}

#endif // FIND_STRING_ON_DIR_H
