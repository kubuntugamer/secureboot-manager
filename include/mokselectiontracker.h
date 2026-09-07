#ifndef MOKSELECTIONTRACKER_H
#define MOKSELECTIONTRACKER_H

#include <QVector>
#include "mokkeyentry.h"

class QTableWidget;
class QTextEdit;

class MokSelectionTracker {
public:
    // Single responsibility: Connect table row highlights straight to your text edit display box
    static void initTracking(QTableWidget *table, QTextEdit *browser, const QVector<MokKeyEntry> &keysCache);
};

#endif // MOKSELECTIONTRACKER_H
