#include "mokgeneratorpage.h"
#include "mokgenerator.h"
#include "mokenrollmentwizard.h"
#include "mokremovalwizard.h"

// Qt Core Layout & Widget Includes
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QTextBrowser>
#include <QProcess>
#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QMessageBox>
#include <QTimer>

MokGeneratorPage::MokGeneratorPage(QWidget *parent)
: QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    mainLayout->addWidget(new QLabel("Common Name (CN) for Security Certificate:", this));
    editGenCommonName = new QLineEdit(this);
    editGenCommonName->setPlaceholderText("e.g., Custom_SecureBoot_Key");
    mainLayout->addWidget(editGenCommonName);

    mainLayout->addWidget(new QLabel("Certificate Validity Duration (Days):", this));
    spinGenDays = new QSpinBox(this);
    spinGenDays->setRange(365, 3650);
    spinGenDays->setValue(365);
    mainLayout->addWidget(spinGenDays);

    btnGenerateMok = new QPushButton("🔑 Finalize and Sign Registration", this);
    btnGenerateMok->setStyleSheet("background-color: #27ae60; color: #ffffff; font-weight: bold; padding: 8px;");

    btnRevokeMok = new QPushButton("⚠️ Launch Automated MOK Deletion Wizard", this);
    btnRevokeMok->setStyleSheet("background-color: #e67e22; color: #ffffff; font-weight: bold; padding: 8px;");

    // Pack the buttons tightly next to each other in a clean horizontal container row
    QHBoxLayout *buttonRowLayout = new QHBoxLayout();
    buttonRowLayout->setSpacing(12);
    buttonRowLayout->addWidget(btnGenerateMok);
    buttonRowLayout->addWidget(btnRevokeMok);

    mainLayout->addLayout(buttonRowLayout);

    mainLayout->addWidget(new QLabel("System Process Execution Monitor Log:", this));
    textGenerationLog = new QTextBrowser(this);
    textGenerationLog->setStyleSheet("background-color: #1e1e24; color: #ffffff; font-family: monospace;");
    mainLayout->addWidget(textGenerationLog);

    connect(btnGenerateMok, &QPushButton::clicked, this, &MokGeneratorPage::executeMokKeyPairGeneration);

    // Connect the danger-zone button to the revocation wizard overlay controller
    connect(btnRevokeMok, &QPushButton::clicked, this, [this]() {
        QString inputCN = editGenCommonName->text().trimmed();

        if (inputCN.isEmpty()) {
            inputCN = "custom_mok_key";
        }

        QString baseFileName = inputCN;
        baseFileName.replace(QRegularExpression("[^a-zA-Z0-9_\\-]"), "_");

        MokRemovalWizard::executeKeyRevocation(inputCN, "", this);
        textGenerationLog->append(QString("\n⚠️ Notice: Revocation requests successfully filed for MOK identity asset: [%1].").arg(baseFileName));
    });
}
void MokGeneratorPage::executeMokKeyPairGeneration()
{
    QString rawInputName = editGenCommonName->text().trimmed();
    if (rawInputName.isEmpty()) {
        textGenerationLog->append("\n⚠️ Aborted: Common Name cannot be blank.");
        return;
    }

    QString baseFileName = rawInputName;
    baseFileName.replace(QRegularExpression("[^a-zA-Z0-9_\\-]"), "_");

    QString outputFolder = QDir::homePath() + "/secureboot-manager-keys";
    QString certPath = QString("%1/%2.der").arg(outputFolder, baseFileName);

    textGenerationLog->append(QString("🚀 Executing local OpenSSL certificate creation for key: [%1]...\n").arg(baseFileName));
    MokGenerator *generator = new MokGenerator(this);
    QString runtimeLogData = "";

    bool keyGenerated = generator->generateKeyPair(rawInputName, baseFileName, spinGenDays->value(), 2048, outputFolder, runtimeLogData);
    textGenerationLog->append(runtimeLogData);

    if (!keyGenerated) {
        textGenerationLog->append("\n❌ Error: Internal cryptographic generation engine failed.");
        return;
    }

    textGenerationLog->append("\n🔒 Staging signature certificate inside sandbox storage array...");
    textGenerationLog->append(QString("Staged target asset: %1").arg(certPath));
    textGenerationLog->append("\n⏳ Launching System Firmware Enrollment Interlock Wizard...");

    QString userDefinedPassword = MokEnrollmentWizard::enforceSystemEnrollment(this);

    if (userDefinedPassword.isEmpty()) {
        textGenerationLog->append("\n🛡️ Failsafe Triggered: System registration safely aborted by user.");
        return;
    }

    textGenerationLog->append("\n📡 User verified parameters. Submitting cryptographic payload tokens via mokutil...");

    // Enforce Rule 1: Strict target binary tokens. Complete separation from shell execution chains.
    QProcess *mokutilProcess = new QProcess(this);
    QString program = QStringLiteral("pkexec");
    QStringList arguments;
    arguments << QStringLiteral("mokutil") << QStringLiteral("--import") << certPath;

    // Securely stream the input password token strings in a safe grouped execution buffer block
    connect(mokutilProcess, &QProcess::started, this, [mokutilProcess, userDefinedPassword]() {
        QByteArray passData = userDefinedPassword.toUtf8() + "\n";
        mokutilProcess->write(passData);
        mokutilProcess->write(passData);
    });

    // FIXED CONNECT PARAMS: Cleaned context tracking signal slots to bypass build blocks
    connect(mokutilProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [this, mokutilProcess, baseFileName](int exitCode, QProcess::ExitStatus status) {

                if (status == QProcess::NormalExit && exitCode == 0) {
                    textGenerationLog->append(QString("\n✨ Success: MOK Import request for [%1] successfully cached into NVRAM storage.").arg(baseFileName));

                    QMessageBox msgBox(this);
                    msgBox.setIcon(QMessageBox::Question);
                    msgBox.setWindowTitle("MOK Registered Successfully");
                    msgBox.setText("<b>The Machine Owner Key is safely staged in NVRAM.</b><br><br>"
                    "Would you like to restart the system now, or defer the reboot until you are finished working?");

                    QPushButton *rebootLater = msgBox.addButton("Reboot Later", QMessageBox::RejectRole);
                    QPushButton *rebootNow = msgBox.addButton("Reboot Now", QMessageBox::AcceptRole);

                    msgBox.setDefaultButton(rebootLater);
                    msgBox.setEscapeButton(rebootLater);
                    rebootLater->setFocus();

                    msgBox.exec();

                    if (msgBox.clickedButton() == rebootNow) {
                        textGenerationLog->append("\n📡 User selected Reboot Now. Please save your work and manually restart via your desktop menu.");
                        QMessageBox::information(this, "Manual Reboot Required",
                                                 "Automated restarts are completely disabled to protect your environment.<br><br>"
                                                 "Please save your open projects and manually restart your machine when you are ready to enroll the key.");
                    } else {
                        textGenerationLog->append("\nℹ️ MOK staging sequence complete. Reboot deferred safely.");
                    }

                } else {
                    QString errorMsg = QString::fromUtf8(mokutilProcess->readAllStandardError()).trimmed();
                    if (errorMsg.isEmpty()) {
                        errorMsg = QString::fromUtf8(mokutilProcess->readAllStandardOutput()).trimmed();
                    }
                    textGenerationLog->append("\n❌ Error: mokutil execution wrapper failed:\n" + errorMsg);
                }
                mokutilProcess->deleteLater();
            });

    mokutilProcess->start(program, arguments);
}
