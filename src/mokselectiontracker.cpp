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
            // Extract the metadata variables along with the full raw data string block
            QString cn = keysCache[currentRow].commonName;
            QString exp = keysCache[currentRow].expirationDate;
            QString sn = keysCache[currentRow].serialNumber;
            QString rawCertText = keysCache[currentRow].rawCertificate;

            // 🎨 HIGH-CONTRAST DATA WRAPPER: Centers metadata overview cards above the full multiline dump
            QString certHtmlWrapper =
            "<div style='font-family: \"Monospace\", \"Courier New\", monospace; color: #a4b0be; padding: 12px; line-height: 1.5; font-size: 13px;'>"
            "  <div style='text-align: center; margin-bottom: 8px;'>"
            "    <span style='color: #ff9f43; font-weight: bold; font-size: 14px;'>📋 X.509 PUBLIC CERTIFICATE DETAILS</span>"
            "  </div>"
            "  <hr style='border: 0; border-top: 1px solid #34495e; margin-bottom: 12px;'>"

            // Meta Information Card Section
            "  <div style='margin-bottom: 12px; line-height: 1.6;'>"
            "    <span style='color: #54a0ff; font-weight: bold;'>Common Name (CN):</span> <span style='color: #ffffff;'> " + cn + "</span><br>"
            "    <span style='color: #54a0ff; font-weight: bold;'>Expiration:</span> <span style='color: #ffaa00;'> " + exp + "</span><br>"
            "    <span style='color: #54a0ff; font-weight: bold;'>Serial Sequence:</span> <span style='color: #00d2d3;'> " + sn + "</span>"
            "  </div>"

            "  <div style='text-align: center; margin: 15px 0 6px 0;'>"
            "    <span style='color: #8395a7; font-size: 11px;'>--- RAW ACTIVE NVRAM SIGNATURE BLOCK ---</span>"
            "  </div>"

            // Raw Certificate Payload Block
            "  <div style='background-color: #1e1e24; border: 1px solid #3f4142; border-radius: 4px; padding: 10px;'>"
            "    <pre style='font-family: \"Monospace\", \"Courier New\", monospace; font-size: 11px; color: #e67e22; margin: 0; white-space: pre-wrap;'>"
            + rawCertText +
            "    </pre>"
            "  </div>"
            "</div>";

            browser->setHtml(certHtmlWrapper);
        }
    });
}
