#include "mokselectiontracker.h"
#include <QTableWidget>
#include <QTextBrowser>

void MokSelectionTracker::initTracking(QTableWidget *table, QTextBrowser *browser, const QVector<MokKeyEntry> &keysCache) {
    if (!table || !browser) return;

    // Use currentItemChanged to perfectly track selections on a data grid table item row!
    QObject::connect(table, &QTableWidget::currentItemChanged, [table, browser, keysCache](QTableWidgetItem *current, QTableWidgetItem *previous) {
        Q_UNUSED(previous);
        if (!current) {
            browser->clear();
            return;
        }

        int currentRow = table->row(current);
        if (currentRow >= 0 && currentRow < keysCache.size()) {
            browser->setPlainText(keysCache[currentRow].rawCertificate);
        } else {
            browser->clear();
        }
    });
}
