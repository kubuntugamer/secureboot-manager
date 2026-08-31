#ifndef MOKUIADAPTER_H
#define MOKUIADAPTER_H

#include <QVector>
#include "mokkeyentry.h"

class QTableWidget;

class MokUiAdapter {
public:
    static void populateTable(QTableWidget *table, const QVector<MokKeyEntry> &keys);
};

#endif // MOKUIADAPTER_H
