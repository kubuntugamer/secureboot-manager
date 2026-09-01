#include "mokuigenerator.h"
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>

void MokUiGenerator::setupGenerationPage(QWidget *pageContainer,
                                         QLineEdit *&commonNameEdit,
                                         QSpinBox *&validityDaysSpin,
                                         QPushButton *&generateBtn,
                                         QTextBrowser *&logBrowser)
{
    if (!pageContainer) return;

    // Force clean any stale implicit layouts
    if (pageContainer->layout()) {
        delete pageContainer->layout();
    }

    QVBoxLayout *masterLayout = new QVBoxLayout(pageContainer);
    pageContainer->setLayout(masterLayout);

    masterLayout->setContentsMargins(25, 25, 25, 25);
    masterLayout->setSpacing(12);

    // 1. Add an explicit Page Header title label block
    QLabel *titleLabel = new QLabel("Machine Owner Key (MOK) Generation Parameters", pageContainer);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(13);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    masterLayout->addWidget(titleLabel);

    // 2. ROW 1: Common Name Input Block
    QVBoxLayout *cnBlock = new QVBoxLayout();
    QLabel *cnLabel = new QLabel("Common Name (CN) / Identity String:", pageContainer);
    commonNameEdit = new QLineEdit(pageContainer);
    commonNameEdit->setText("KDE Secure Boot Manager Key");
    commonNameEdit->setMinimumHeight(32);
    cnBlock->addWidget(cnLabel);
    cnBlock->addWidget(commonNameEdit);
    masterLayout->addLayout(cnBlock);

    // 3. ROW 2: Validity Days Entry Block
    QVBoxLayout *daysBlock = new QVBoxLayout();
    QLabel *daysLabel = new QLabel("Certificate Validity Lifespan (Days):", pageContainer);
    validityDaysSpin = new QSpinBox(pageContainer);
    validityDaysSpin->setRange(1, 365000);
    validityDaysSpin->setValue(36500); // 100-year developer testing lifecycle criteria
    validityDaysSpin->setMinimumHeight(32);
    daysBlock->addWidget(daysLabel);
    daysBlock->addWidget(validityDaysSpin);
    masterLayout->addLayout(daysBlock);

    // 4. ROW 3: The Action Execution Trigger Button
    generateBtn = new QPushButton("🛠️ Generate Cryptographic Key Pair", pageContainer);
    generateBtn->setMinimumHeight(40);
    QFont btnFont = generateBtn->font();
    btnFont.setBold(true);
    generateBtn->setFont(btnFont);
    masterLayout->addWidget(generateBtn);

    // 5. ROW 4: Monospaced Diagnostics Terminal Browser Window
    QLabel *logLabel = new QLabel("OpenSSL System Execution Log Terminal:", pageContainer);
    logBrowser = new QTextBrowser(pageContainer);
    logBrowser->setReadOnly(true);

    QFont monoFont("monospace");
    monoFont.setStyleHint(QFont::Monospace);
    logBrowser->setFont(monoFont);

    masterLayout->addWidget(logLabel);
    masterLayout->addWidget(logBrowser);

    // ✨ THE FIX: Explicitly assign a high layout stretch factor weight to the text log browser box!
    // This forces the input rows above to group up tightly, giving the form proper spatial balance.
    masterLayout->setStretchFactor(logBrowser, 1);

    pageContainer->updateGeometry();
}
