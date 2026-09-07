#ifndef MOKLEDGERPAGE_H
#define MOKLEDGERPAGE_H

#include <QWidget>
#include <QVector>
#include "mokkeyentry.h"

class QTableWidget;

class MokLedgerPage : public QWidget
{
    Q_OBJECT
public:
    // 🔒 UNIFIED SIGNATURE: Expects the data pool passed directly from the main layout window manager
    explicit MokLedgerPage(const QVector<MokKeyEntry> &keys, QWidget *parent = nullptr);
    ~MokLedgerPage() override = default;

    QTableWidget* getTableWidget() const;

private:
    QTableWidget *tableEnrolledKeys;
};

#endif // MOKLEDGERPAGE_H
