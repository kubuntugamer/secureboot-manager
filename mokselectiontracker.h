#ifndef MOKSELECTIONTRACKER_H
#define MOKSELECTIONTRACKER_H

#include <QVector>
#include "mokkeyentry.h"

class QTableWidget;
class QTextBrowser;

class MokSelectionTracker {
public:
    // Single responsibility: Connect table row highlights straight to your text browser box
    static void initTracking(QTableWidget *table, QTextBrowser *browser, const QVector<MokKeyEntry> &keysCache);
};

#endif // MOKSELECTIONTRACKER_H
