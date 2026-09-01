#include "mokselectiontracker.h"
#include <QTableWidget>
#include <QTextBrowser>

void MokSelectionTracker::initTracking(QTableWidget *table, QTextBrowser *browser, const QVector<MokKeyEntry> &keysCache) {
    if (!table || !browser) return;

    QObject::connect(table, &QTableWidget::itemClicked, [table, browser, keysCache](QTableWidgetItem *item) {
        if (!item) {
            return;
        }

        int currentRow = table->row(item);
        if (currentRow >= 0 && currentRow < keysCache.size()) {
            QString rawCertText = keysCache[currentRow].rawCertificate;

            QString certHtmlWrapper =
            "<div style='font-family: sans-serif; color: #bdc3c7; padding: 10px; line-height: 1.4;'>"
            "  <div style='text-align: center; margin-bottom: 12px;'>"
            "    <span style='font-size: 32px;'>📋</span>"
            "    <h3 style='color: #ffffff; margin: 5px 0 2px 0; font-size: 14px;'>X.509 Public Certificate</h3>"
            "    <span style='font-size: 10px; color: #3498db;'>Raw Active NVRAM Signature Block</span>"
            "  </div>"
            "  <hr style='border: 0; border-top: 1px solid #3f4142; margin-bottom: 12px;'>"
            "  <div style='background-color: #1e1e24; border: 1px solid #3f4142; border-radius: 4px; padding: 8px;'>"
            "    <pre style='font-family: monospace; font-size: 10px; color: #e67e22; margin: 0; white-space: pre-wrap;'>"
            + rawCertText +
            "    </pre>"
            "  </div>"
            "</div>";

            browser->setHtml(certHtmlWrapper);
        }
    });
}
