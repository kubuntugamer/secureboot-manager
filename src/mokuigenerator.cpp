#include "mokuigenerator.h"
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>

void MokUiGenerator::setupGenerationPage(QWidget *pageContainer,
                                         QLineEdit *&commonNameEdit,
                                         QSpinBox *&validityDaysSpin,
                                         QPushButton *&generateBtn,
                                         QTextBrowser *&logBrowser)
{
    if (!pageContainer) return;

    if (pageContainer->layout()) {
        delete pageContainer->layout();
    }

    QVBoxLayout *masterLayout = new QVBoxLayout(pageContainer);
    pageContainer->setLayout(masterLayout);
    masterLayout->setContentsMargins(25, 25, 25, 25);
    masterLayout->setSpacing(12);

    QLabel *titleLabel = new QLabel("Machine Owner Key (MOK) Generation Parameters", pageContainer);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(13);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    masterLayout->addWidget(titleLabel);

    // Common Name Input Area
    QVBoxLayout *cnBlock = new QVBoxLayout();
    QLabel *cnLabel = new QLabel("Common Name (CN) / Identity String:", pageContainer);
    commonNameEdit = new QLineEdit(pageContainer);
    commonNameEdit->setPlaceholderText("e.g., Custom Secure Boot Module Key");
    commonNameEdit->setMinimumHeight(32);
    cnBlock->addWidget(cnLabel);
    cnBlock->addWidget(commonNameEdit);
    masterLayout->addLayout(cnBlock);

    // Validity Days Input Area
    QVBoxLayout *daysBlock = new QVBoxLayout();
    QLabel *daysLabel = new QLabel("Certificate Validity Lifespan (Days):", pageContainer);
    validityDaysSpin = new QSpinBox(pageContainer);
    validityDaysSpin->setRange(1, 365000);
    validityDaysSpin->setValue(36500);
    validityDaysSpin->setMinimumHeight(32);
    daysBlock->addWidget(daysLabel);
    daysBlock->addWidget(validityDaysSpin);
    masterLayout->addLayout(daysBlock);

    // Side-by-side Dual Action Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    generateBtn = new QPushButton("🛠️ Generate Cryptographic Key Pair", pageContainer);
    generateBtn->setMinimumHeight(40);
    QFont btnFont = generateBtn->font();
    btnFont.setBold(true);
    generateBtn->setFont(btnFont);

    QPushButton *revokeBtn = new QPushButton("🗑️ Revoke Delete MOK Key", pageContainer);
    revokeBtn->setMinimumHeight(40);
    revokeBtn->setFont(btnFont);
    revokeBtn->setObjectName("btnRevokeMok"); // Object label mapping identifier for the page controller

    buttonLayout->addWidget(generateBtn);
    buttonLayout->addWidget(revokeBtn);
    masterLayout->addLayout(buttonLayout);

    QLabel *logLabel = new QLabel("OpenSSL System Execution Log Terminal:", pageContainer);
    logBrowser = new QTextBrowser(pageContainer);
    logBrowser->setReadOnly(true);
    QFont monoFont("monospace");
    monoFont.setStyleHint(QFont::Monospace);
    logBrowser->setFont(monoFont);

    masterLayout->addWidget(logLabel);
    masterLayout->addWidget(logBrowser);
    masterLayout->setStretchFactor(logBrowser, 1);

    pageContainer->updateGeometry();
}
