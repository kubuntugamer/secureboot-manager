#include "mokuigenerator.h"
#include "mokgenerator.h"
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QDir>

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

    QVBoxLayout *cnBlock = new QVBoxLayout();
    QLabel *cnLabel = new QLabel("Common Name (CN) / Identity String:", pageContainer);
    commonNameEdit = new QLineEdit(pageContainer);
    commonNameEdit->setText("KDE Secure Boot Manager Key");
    commonNameEdit->setMinimumHeight(32);
    cnBlock->addWidget(cnLabel);
    cnBlock->addWidget(commonNameEdit);
    masterLayout->addLayout(cnBlock);

    QVBoxLayout *daysBlock = new QVBoxLayout();
    QLabel *daysLabel = new QLabel("Certificate Validity Lifespan (Days):", pageContainer);
    validityDaysSpin = new QSpinBox(pageContainer);
    validityDaysSpin->setRange(1, 365000);
    validityDaysSpin->setValue(36500);
    validityDaysSpin->setMinimumHeight(32);
    daysBlock->addWidget(daysLabel);
    daysBlock->addWidget(validityDaysSpin);
    masterLayout->addLayout(daysBlock);

    generateBtn = new QPushButton("🛠️ Generate Cryptographic Key Pair", pageContainer);
    generateBtn->setMinimumHeight(40);
    QFont btnFont = generateBtn->font();
    btnFont.setBold(true);
    generateBtn->setFont(btnFont);
    masterLayout->addWidget(generateBtn);

    QLabel *logLabel = new QLabel("OpenSSL System Execution Log Terminal:", pageContainer);
    logBrowser = new QTextBrowser(pageContainer);
    logBrowser->setReadOnly(true);
    QFont monoFont("monospace");
    monoFont.setStyleHint(QFont::Monospace);
    logBrowser->setFont(monoFont);

    masterLayout->addWidget(logLabel);
    masterLayout->addWidget(logBrowser);
    masterLayout->setStretchFactor(logBrowser, 1);

    // 🔒 OOP Encapsulation: The Submodule completely binds and manages its own actions
    QObject::connect(generateBtn, &QPushButton::clicked, pageContainer, [commonNameEdit, validityDaysSpin, logBrowser, pageContainer]() {
        logBrowser->clear();
        QString commonName = commonNameEdit->text().trimmed();
        int validityDays = validityDaysSpin->value();
        QString outputFolder = QDir::homePath() + "/secureboot-manager-keys";

        if (commonName.isEmpty()) {
            logBrowser->append("❌ Error: Common Name cannot be empty.");
            return;
        }

        logBrowser->append("🚀 Executing local OpenSSL certificate creation...\n");
        MokGenerator *generator = new MokGenerator(pageContainer);
        QString runtimeLogData = "";

        bool keyGenerated = generator->generateKeyPair(commonName, validityDays, 2048, outputFolder, runtimeLogData);
        logBrowser->append(runtimeLogData);

        if (keyGenerated) {
            logBrowser->append("\n🔒 Staging signature certificate inside UEFI database...");
            QString certPath = outputFolder + "/MOK.der";
            logBrowser->append(QString("Staged target asset: %1").arg(certPath));
            logBrowser->append("\n\n🛑 ATTENTION USER: A system reboot is required to proceed!\n"
            "Please restart your computer now. The blue UEFI MokManager screen "
            "will guide you through entering your password to authorize your enrollment key. "
            "Once back at your desktop, you are fully authorized to install your alternative kernel choices.");
        }
    });

    pageContainer->updateGeometry();
}
