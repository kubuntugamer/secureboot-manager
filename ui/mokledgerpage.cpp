#include "mokledgerpage.h"
#include "mokprovider.h"
#include "mokuiadapter.h"
#include "mokselectiontracker.h"
#include "mokkeyentry.h"
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QVector>

MokLedgerPage::MokLedgerPage(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(0);

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
        "QTableWidget::item { qproperty-textAlignment: 'AlignCenter'; }" // Forces item alignment fallback mechanics
    );

    mainLayout->addWidget(tableEnrolledKeys);

    // Load live system NVRAM certificates directly into the table cells
    QVector<MokKeyEntry> activeKeys = MokProvider::getLiveKeys();
    MokUiAdapter::populateTable(tableEnrolledKeys, activeKeys);
}

// 📦 PUBLIC ACCESS SPECIFIER: Lets the main window hook its sidebar tracker up to this table widget
QTableWidget* MokLedgerPage::getTableWidget() const
{
    return tableEnrolledKeys;
}
