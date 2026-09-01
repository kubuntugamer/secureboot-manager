#include "mokselectiontracker.h"
#include <QTableWidget>
#include <QTextBrowser>

void MokSelectionTracker::initTracking(QTableWidget *table, QTextBrowser *browser, const QVector<MokKeyEntry> &keysCache) {
    if (!table || !browser) return;

    // CRITICAL TRACKING UPGRADE: Connect to direct mouse interaction signals (itemClicked)
    // instead of layout focus movement paths (currentItemChanged). This guarantees updates
    // while maintaining your professional NoFocus startup style policy.
    QObject::connect(table, &QTableWidget::itemClicked, [table, browser, keysCache](QTableWidgetItem *item) {
        if (!item) {
            return;
        }

        int currentRow = table->row(item);
        if (currentRow >= 0 && currentRow < keysCache.size()) {
            browser->setPlainText(keysCache[currentRow].rawCertificate);
        }
    });
}
