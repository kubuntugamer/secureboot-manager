#include "mokledgerpage.h"
#include "mokuiadapter.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QProcess>

MokLedgerPage::MokLedgerPage(const QVector<MokKeyEntry> &keys, QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(10); // Added spacing to give the status bar breathing room

    tableEnrolledKeys = new QTableWidget(this);

    // 📐 TABLE REAL-ESTATE POLISH: Scale cells across the full widescreen canvas area
    tableEnrolledKeys->setColumnCount(3);
    tableEnrolledKeys->setHorizontalHeaderLabels({"Common Name (CN)", "Expiration Date", "Serial Number"});

    tableEnrolledKeys->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tableEnrolledKeys->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tableEnrolledKeys->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    tableEnrolledKeys->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableEnrolledKeys->setAlternatingRowColors(true);

    // 🎯 CENTER ALIGNMENT POLISH: Center-align header titles instead of clipping them left
    tableEnrolledKeys->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

    // 🎨 CUSTOM STYLING: Force crisp padding margins and apply uniform dark borders + centered text rules
    tableEnrolledKeys->setStyleSheet(
        "QHeaderView::section { font-weight: bold; background-color: #2c3e50; color: #ffffff; border: 1px solid #3f4142; }"
        "QTableWidget { background-color: #1e1e24; gridline-color: #2c3e50; color: #ffffff; text-align: center; }"
        "QTableWidget::item { qproperty-textAlignment: 'AlignCenter'; }"
    );

    // Drop the table widget first into the layout stack
    mainLayout->addWidget(tableEnrolledKeys);

    // 🛡️ RE-IMPLEMENT SECURE BOOT STATUS BAR UNDER THE TABLE
    QHBoxLayout *statusBarLayout = new QHBoxLayout();
    QLabel *statusTitleLabel = new QLabel("<b>System Security Status:</b>", this);
    QLabel *statusValueLabel = new QLabel(this);

    statusTitleLabel->setStyleSheet("color: #ffffff; font-size: 12px;");

    // Query the host firmware layer state securely
    QProcess sbCheck;
    sbCheck.start("mokutil", QStringList() << "--sb-state");
    sbCheck.waitForFinished();
    QString sbOutput = QString::fromLocal8Bit(sbCheck.readAllStandardOutput()).trimmed();

    if (sbOutput.contains("SecureBoot enabled")) {
        statusValueLabel->setText("🔒 <font color='#2ecc71'><b>SECURE BOOT ACTIVE</b></font>");
    } else if (sbOutput.contains("SecureBoot disabled")) {
        statusValueLabel->setText("🔓 <font color='#e74c3c'><b>SECURE BOOT INACTIVE</b></font>");
    } else {
        statusValueLabel->setText("⚠️ <font color='#f1c40f'><b>SECURE BOOT UNSUPPORTED</b></font>");
    }

    statusBarLayout->addWidget(statusTitleLabel);
    statusBarLayout->addWidget(statusValueLabel);
    statusBarLayout->addStretch(); // Aligns the text cleanly to the left edge

    // Inject the status container layout below the data grid target
    mainLayout->addLayout(statusBarLayout);

    // ⚡ CONSOLIDATED CELL DELIVERY: Populates the grid using the shared, synchronized memory vector
    MokUiAdapter::populateTable(tableEnrolledKeys, keys);
}

QTableWidget* MokLedgerPage::getTableWidget() const
{
    return tableEnrolledKeys;
}
