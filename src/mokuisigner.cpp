#include "mokuisigner.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QButtonGroup>
#include <QDir> // 🔍 Added missing header for system path navigation checks

void MokUiSigner::setupSigningPage(QWidget *pageContainer, QLineEdit *&targetPathEdit, QLineEdit *&keyPathEdit,
                                   QPushButton *&browseBinaryBtn, QPushButton *&executeSignBtn,
                                   const QStringList &unsignedLabels, const QStringList &unsignedPaths, const QStringList &signedLabels)
{
    if (!pageContainer) return;

    // Master Layout Grid Configuration
    QVBoxLayout *mainLayout = new QVBoxLayout(pageContainer);
    mainLayout->setContentsMargins(30, 25, 30, 25);
    mainLayout->setSpacing(14);

    QLabel *titleLabel = new QLabel("Wizard-Driven Kernel Secure Boot Signer", pageContainer);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #ffffff;");
    mainLayout->addWidget(titleLabel);

    QLabel *subLabel = new QLabel("The system has scanned your package repository database. Staged targets require signature injection, while verified kernels are already fully authorized by your local MOK or vendor keys.", pageContainer);
    subLabel->setStyleSheet("font-size: 11px; color: #bdc3c7;");
    subLabel->setWordWrap(true);
    mainLayout->addWidget(subLabel);

    mainLayout->addSpacing(5);

    // Split columns for Actionable vs Verified items
    QHBoxLayout *columnsLayout = new QHBoxLayout();
    columnsLayout->setSpacing(20);

    // Left Column: Staged/Unsigned Action Targets
    QVBoxLayout *leftCol = new QVBoxLayout();
    leftCol->setSpacing(10);
    QLabel *leftHeader = new QLabel("⚠️ Staged for Signature Injection:", pageContainer);
    leftHeader->setStyleSheet("font-size: 11px; font-weight: bold; color: #ff9f43;");
    leftCol->addWidget(leftHeader);

    // Right Column: Already Verified/Protected Items
    QVBoxLayout *rightCol = new QVBoxLayout();
    rightCol->setSpacing(10);
    QLabel *rightHeader = new QLabel("🔒 Currently Verified & Protected:", pageContainer);
    rightHeader->setStyleSheet("font-size: 11px; font-weight: bold; color: #1dd1a1;");
    rightCol->addWidget(rightHeader);

    QButtonGroup *cardGroup = new QButtonGroup(pageContainer);
    cardGroup->setExclusive(true);

    int unsignedCount = 0;
    QString firstUnsignedPath = "";

    // Paint Left Column: Render clickable cards
    for (int i = 0; i < unsignedLabels.size(); ++i) {
        unsignedCount++;
        QString displayLabel = unsignedLabels.at(i);
        QString absolutePath = unsignedPaths.at(i);

        QPushButton *btnKernelCard = new QPushButton(QString("🐧 %1\nStaged for MOK Signing").arg(displayLabel), pageContainer);
        btnKernelCard->setStyleSheet(
            "QPushButton {"
            "  background-color: #1e272e; color: #ffffff; border: 1px solid #3f4142; border-radius: 6px;"
            "  padding: 12px; text-align: center; font-family: monospace; font-size: 11px; line-height: 1.4;"
            "}"
            "QPushButton:hover { background-color: #2c3e50; border-color: #3daee9; }"
            "QPushButton:checked { background-color: #2a3b4d; border: 2px solid #3daee9; color: #ffffff; }"
        );
        btnKernelCard->setCheckable(true);

        if (unsignedCount == 1) {
            btnKernelCard->setChecked(true);
            firstUnsignedPath = absolutePath;
        }

        cardGroup->addButton(btnKernelCard);
        leftCol->addWidget(btnKernelCard);

        // Save the chosen path straight into the parent page property on click
        QObject::connect(btnKernelCard, &QPushButton::clicked, pageContainer, [pageContainer, absolutePath]() {
            pageContainer->setProperty("selectedKernelPath", absolutePath);
        });
    }

    // Set default initial selection track
    pageContainer->setProperty("selectedKernelPath", firstUnsignedPath);

    // Paint Right Column: Render protected, static cards
    for (int i = 0; i < signedLabels.size(); ++i) {
        QString displayLabel = signedLabels.at(i);

        QPushButton *btnSignedCard = new QPushButton(QString("🔒 %1\nVerified & Protected").arg(displayLabel), pageContainer);
        btnSignedCard->setStyleSheet(
            "QPushButton {"
            "  background-color: #15191c; color: #8395a7; border: 1px dashed #1e3d30; border-radius: 6px;"
            "  padding: 12px; text-align: center; font-family: monospace; font-size: 11px; line-height: 1.4;"
            "}"
        );
        btnSignedCard->setEnabled(false);
        rightCol->addWidget(btnSignedCard);
    }

    // Handle Empty States
    if (unsignedCount == 0) {
        QLabel *emptyLabel = new QLabel("🔒 All kernels verified.\nNo actions required.", pageContainer);
        emptyLabel->setStyleSheet("font-size: 11px; color: #1dd1a1; font-family: monospace; text-align: center; padding: 15px; border: 1px dashed #1e3d30; border-radius: 6px;");
        leftCol->addWidget(emptyLabel);
    }

    if (signedLabels.isEmpty()) {
        QLabel *emptyLabel = new QLabel("⚠️ No secure signatures\ndetected on disk.", pageContainer);
        emptyLabel->setStyleSheet("font-size: 11px; color: #ff9f43; font-family: monospace; text-align: center; padding: 15px; border: 1px dashed #3f4142; border-radius: 6px;");
        rightCol->addWidget(emptyLabel);
    }

    leftCol->addStretch();
    rightCol->addStretch();

    columnsLayout->addLayout(leftCol, 1);
    columnsLayout->addLayout(rightCol, 1);
    mainLayout->addLayout(columnsLayout);

    mainLayout->addSpacing(5);
    mainLayout->addWidget(new QLabel("<hr style='border: 0; border-top: 1px solid #3f4142;'>", pageContainer));

    // Nullify targetPathEdit reference since we are tracking via parent properties now
    targetPathEdit = nullptr;

    keyPathEdit = new QLineEdit(pageContainer);
    keyPathEdit->setStyleSheet("background-color: #1e1e24; color: #ffffff; border: 1px solid #3f4142; border-radius: 4px; padding: 6px; font-size: 11px;");

    // 🔍 DYNAMIC SYSTEM KEY LOOKUP SCANNER
    QStringList structuralSearchPaths;
    structuralSearchPaths << "/var/lib/shim-signed/mok";                           // 💎 Prioritize active system Liquorix setup folder
    structuralSearchPaths << QDir::cleanPath(QDir::homePath() + "/.config/secureboot-manager/keys");
    structuralSearchPaths << QDir::cleanPath(QDir::homePath() + "/MOK");
    structuralSearchPaths << QDir::homePath();

    QString fullyResolvedDiscoveredKey = "";

    for (const QString &searchPath : structuralSearchPaths) {
        QDir checkDir(searchPath);
        if (!checkDir.exists()) continue;

        QStringList keyFilters;
        keyFilters << "MOK.priv" << "MOK.key" << "*.priv" << "*.key";
        QStringList foundFiles = checkDir.entryList(keyFilters, QDir::Files);

        if (!foundFiles.isEmpty()) {
            QString targetFile = foundFiles.first();
            QString rawAbsolutePath = checkDir.absoluteFilePath(targetFile);

            // Cleanly format home paths to use the short standard "~/" notation syntax view
            if (rawAbsolutePath.startsWith(QDir::homePath())) {
                fullyResolvedDiscoveredKey = "~" + rawAbsolutePath.mid(QDir::homePath().length());
            } else {
                fullyResolvedDiscoveredKey = rawAbsolutePath;
            }
            break;
        }
    }

    // Seed the text field with the detected key file, fallback if none resolved
    if (!fullyResolvedDiscoveredKey.isEmpty()) {
        keyPathEdit->setText(fullyResolvedDiscoveredKey);
    } else {
        keyPathEdit->setText("~/.config/secureboot-manager/keys/MOK.key");
    }

    mainLayout->addWidget(new QLabel("Staged Authorization Key Asset Path:", pageContainer));
    mainLayout->addWidget(keyPathEdit);

    executeSignBtn = new QPushButton("Inject Secure Boot Signature", pageContainer);
    executeSignBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #2a3b4d; color: #3daee9; border: 1px solid #3daee9;"
        "  font-weight: bold; padding: 12px; border-radius: 4px; font-size: 11px;"
        "}"
        "QPushButton:hover { background-color: #3daee9; color: #ffffff; }"
    );
    mainLayout->addWidget(executeSignBtn);
    mainLayout->addStretch();
}
