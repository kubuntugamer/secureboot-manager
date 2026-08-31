#include "mokuiadapter.h"
#include <QTableWidget>
#include <QTableWidgetItem>

void MokUiAdapter::populateTable(QTableWidget *table, const QVector<MokKeyEntry> &keys) {
    if (!table) return;
    table->setRowCount(keys.size());
    for (int row = 0; row < keys.size(); ++row) {
        table->setItem(row, 0, new QTableWidgetItem(keys[row].commonName));
        table->setItem(row, 1, new QTableWidgetItem(keys[row].expirationDate));
        table->setItem(row, 2, new QTableWidgetItem(keys[row].serialNumber));
    }
}
