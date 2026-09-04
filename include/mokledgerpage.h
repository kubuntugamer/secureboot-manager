#ifndef MOKLEDGERPAGE_H
#define MOKLEDGERPAGE_H

#include <QWidget>

class QTableWidget;

class MokLedgerPage : public QWidget
{
    Q_OBJECT
public:
    explicit MokLedgerPage(QWidget *parent = nullptr);
    QTableWidget* getTableWidget() const; // 🔍 Added helper getter function

private:
    QTableWidget *tableEnrolledKeys;
};

#endif // MOKLEDGERPAGE_H
