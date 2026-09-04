#include "mokuigenerator.h"
#include "mokgenerator.h"
#include "mokenrollmentwizard.h"
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QDir>
#include <QProcess>

// Forward declare custom blockade overlay function
void launchBlockadeRebootOverlay(QWidget *parent);

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

    // Common Name Input Layout
    QVBoxLayout *cnBlock = new QVBoxLayout();
    QLabel *cnLabel = new QLabel("Common Name (CN) / Identity String:", pageContainer);
    commonNameEdit = new QLineEdit(pageContainer);
    commonNameEdit->setPlaceholderText("e.g., Custom Secure Boot Module Key");
    commonNameEdit->setMinimumHeight(32);
    cnBlock->addWidget(cnLabel);
    cnBlock->addWidget(commonNameEdit);
    masterLayout->addLayout(cnBlock);

    // Validity Days Layout
    QVBoxLayout *daysBlock = new QVBoxLayout();
    QLabel *daysLabel = new QLabel("Certificate Validity Lifespan (Days):", pageContainer);
    validityDaysSpin = new QSpinBox(pageContainer);
    validityDaysSpin->setRange(1, 365000);
    validityDaysSpin->setValue(36500);
    validityDaysSpin->setMinimumHeight(32);
    daysBlock->addWidget(daysLabel);
    daysBlock->addWidget(validityDaysSpin);
    masterLayout->addLayout(daysBlock);

    // Action button drawn clean and tight without password clutter
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

    // Binds and manages page generation actions
    QObject::connect(generateBtn, &QPushButton::clicked, pageContainer, [commonNameEdit, validityDaysSpin, logBrowser, pageContainer]() {
        logBrowser->clear();

        // 💎 TARGET YOUR RECOGNIZED SYSTEM LIQUORIX SETTING PATH DIRECTLY FIRST
        QString outputFolder = "/var/lib/shim-signed/mok";
        QString certPath = outputFolder + "/MOK.der";

        // 🔍 STEP 1: RUN PRE-FLIGHT FIRMWARE CHECK IMMEDIATELY BEFORE ANY VALIDATIONS
        bool isAlreadyEnrolled = false;
        if (QFile::exists(certPath)) {
            QProcess checkProcess;
            checkProcess.start("mokutil", QStringList() << "--test-key" << certPath);
            checkProcess.waitForFinished();
            if (checkProcess.exitCode() == 0) {
                isAlreadyEnrolled = true;
            }
        }

        // 🟢 FLOW A: KEY IS ALREADY ENROLLED IN SYSTEM FIRMWARE
        if (isAlreadyEnrolled) {
            logBrowser->append("ℹ️ System Check: This MOK certificate is already trusted and active in your firmware.");
            logBrowser->append("✅ Safe Bypass Active: System security rings verified. No file generation or wizard enrollment is required for this workstation environment.");
            return; // Exit out of the loop safely! Zero files generated, zero popups thrown.
        }

        // 🔴 FLOW B: NEW OR UNTRUSTED KEY WORKFLOW ACTIONS
        QString commonName = commonNameEdit->text().trimmed();
        int validityDays = validityDaysSpin->value();

        // Check fallback dir path if default firmware directory isn't accessible yet
        if (!QDir(outputFolder).exists()) {
            outputFolder = QDir::homePath() + "/secureboot-manager-keys";
            certPath = outputFolder + "/MOK.der";
        }

        if (commonName.isEmpty()) {
            // Apply a placeholder string instead of throwing a disruptive validation dialog message box!
            commonName = "KDE Secure Boot Manager Key";
        }

        logBrowser->append("🚀 Executing local OpenSSL certificate creation...\n");
        MokGenerator *generator = new MokGenerator(pageContainer);
        QString runtimeLogData = "";

        bool keyGenerated = generator->generateKeyPair(commonName, validityDays, 2048, outputFolder, runtimeLogData);
        logBrowser->append(runtimeLogData);

        if (keyGenerated) {
            logBrowser->append("\n🔒 Staging signature certificate inside UEFI database...");
            logBrowser->append(QString("Staged target asset: %1").arg(certPath));

            logBrowser->append("\n⏳ Launching System Firmware Enrollment Interlock Wizard...");

            QString userDefinedPassword = MokEnrollmentWizard::enforceSystemEnrollment(pageContainer);

            if (userDefinedPassword.isEmpty()) {
                logBrowser->append("\n🛡️ Failsafe Triggered: System registration safely aborted by user. Key file remains on disk but is NOT registered with firmware.");
                return;
            }

            logBrowser->append("\n📡 User verified parameters. Submitting cryptographic payload tokens via mokutil...");

            QProcess *mokutilProcess = new QProcess(pageContainer);
            QStringList runArgs;
            runArgs << "--import" << certPath;

            QObject::connect(mokutilProcess, &QProcess::started, pageContainer, [mokutilProcess, userDefinedPassword]() {
                mokutilProcess->write(userDefinedPassword.toUtf8() + "\n");
                mokutilProcess->write(userDefinedPassword.toUtf8() + "\n");
            });

            QObject::connect(mokutilProcess, &QProcess::finished, pageContainer, [pageContainer, mokutilProcess, logBrowser](int exitCode, QProcess::ExitStatus status) {
                if (status == QProcess::NormalExit && exitCode == 0) {
                    logBrowser->append("\n✨ Success: MOK Import request successfully cached into NVRAM storage.");
                    launchBlockadeRebootOverlay(pageContainer);
                } else {
                    QString errorMsg = QString::fromUtf8(mokutilProcess->readAllStandardError());
                    logBrowser->append("\n❌ Error: mokutil execution wrapper failed:\n" + errorMsg);
                }
                mokutilProcess->deleteLater();
            });

            mokutilProcess->start("pkexec", QStringList() << "mokutil" << runArgs);
        }
    });

    pageContainer->updateGeometry();
}
