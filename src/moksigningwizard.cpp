#include "moksigningwizard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QCheckBox>
#include <QComboBox>
#include <QStackedWidget>
#include <QMessageBox>
#include <QProcess>
#include <QDir>
#include <QFile>
#include <QTimer>

MokSigningWizard::MokSigningWizard(const QString &kernelVersion, QWidget *parent)
: QDialog(parent), targetKernelVersion(kernelVersion)
{
    this->setWindowTitle("🔒 WIZARD-DRIVEN SECURE BOOT SIGNING PIPELINE");
    this->setFixedSize(580, 460);
    this->setModal(true);

    sandboxKeyDir = QDir::homePath() + "/secureboot-manager-keys";
    targetKernelPath = QString("/boot/vmlinuz-%1").arg(targetKernelVersion);

    wizardStack = new QStackedWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(wizardStack);

    buildVerificationView();
    buildExecutionTerminalView();
    buildAutomationHookView();
}

void MokSigningWizard::buildVerificationView()
{
    QWidget *page = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    QLabel *title = new QLabel("📋 STEP 1: CONSOLIDATED SIGNING ASSETS VERIFICATION", page);
    title->setStyleSheet("color: #54a0ff; font-weight: bold; font-size: 13px;");
    layout->addWidget(title);

    lblAssetVerification = new QLabel(QString("<b>Target Binary Path:</b> %1").arg(targetKernelPath), page);
    lblAssetVerification->setWordWrap(true);
    layout->addWidget(lblAssetVerification);

    layout->addWidget(new QLabel("<b>Select Signing Key Profile:</b>", page));

    keySelectorComboBox = new QComboBox(page);
    keySelectorComboBox->setMinimumHeight(32);

    QDir keyDir(sandboxKeyDir);
    QStringList localCerts = keyDir.entryList(QStringList() << "*.der", QDir::Files);
    for (const QString &certFile : localCerts) {
        keySelectorComboBox->addItem(QIcon::fromTheme("dialog-password"), certFile.section('.', 0, 0));
    }
    layout->addWidget(keySelectorComboBox);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnCancel = new QPushButton("Cancel", page);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    // FIX: Parented button widget instantiation cleanly to 'page' to stop memory drift layout anomalies
    btnStartSigning = new QPushButton("🚀 Inject Cryptographic Signature", page);
    btnStartSigning->setStyleSheet("background-color: #2980b9; color: #ffffff; font-weight: bold; min-height: 35px;");
    connect(btnStartSigning, &QPushButton::clicked, this, &MokSigningWizard::processSignatureInjection);

    btnLayout->addWidget(btnCancel);
    btnStartSigning->setFont(QFont("", -1, QFont::Bold));
    btnLayout->addWidget(btnStartSigning);
    layout->addLayout(btnLayout);

    wizardStack->addWidget(page);
}

void MokSigningWizard::buildExecutionTerminalView()
{
    QWidget *page = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(12);

    QLabel *title = new QLabel("⚡ STEP 2: PRIVILEGED SIGNING TERMINAL STREAM", page);
    title->setStyleSheet("color: #e67e22; font-weight: bold; font-size: 13px;");
    layout->addWidget(title);

    terminalLogView = new QTextBrowser(page);
    terminalLogView->setStyleSheet("background-color: #1e1e24; color: #ffffff; font-family: monospace;");
    layout->addWidget(terminalLogView);

    // FIX: Parented push button execution transition widget properly to 'page'
    btnProceedToAutomation = new QPushButton("Continue to Automation Configuration ➡️", page);
    btnProceedToAutomation->setEnabled(false);
    btnProceedToAutomation->setMinimumHeight(35);
    connect(btnProceedToAutomation, &QPushButton::clicked, this, [this]() { wizardStack->setCurrentIndex(2); });
    layout->addWidget(btnProceedToAutomation);

    wizardStack->addWidget(page);
}

void MokSigningWizard::buildAutomationHookView()
{
    QWidget *page = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    QLabel *title = new QLabel("🤖 STEP 3: AUTOMATIC SIGNING SYSTEM INTEGRATION", page);
    title->setStyleSheet("color: #10ac84; font-weight: bold; font-size: 13px;");
    layout->addWidget(title);

    QLabel *desc = new QLabel(
        "To guarantee that future system upgrades do not overwrite or break your Secure Boot verification loops, "
        "the wizard can install a permanent kernel hook utility directly onto your machine script directories.<br><br>"
        "<b>Enabling this feature deploys a post-installation filter script inside /etc/kernel/postinst.d/ "
        "to automatically intercept and sign new incoming Linux kernels on the fly.</b>", page);
    desc->setWordWrap(true);
    layout->addWidget(desc);

    chkInstallSystemHook = new QCheckBox("Deploy Automated Kernel Post-Install Hooks Daemon", page);
    chkInstallSystemHook->setChecked(true);
    chkInstallSystemHook->setStyleSheet("font-weight: bold; color: #ff9f43;");
    layout->addWidget(chkInstallSystemHook);

    btnCompleteWizard = new QPushButton("🏁 Finalize Wizard & Apply Changes", page);
    btnCompleteWizard->setStyleSheet("background-color: #10ac84; color: #ffffff; font-weight: bold; min-height: 38px;");
    connect(btnCompleteWizard, &QPushButton::clicked, this, &MokSigningWizard::finalizeAutomationDeployment);
    layout->addWidget(btnCompleteWizard);

    wizardStack->addWidget(page);
}

void MokSigningWizard::processSignatureInjection()
{
    if (keySelectorComboBox->currentIndex() == -1) {
        QMessageBox::warning(this, "Key Missing", "No cryptographic signing keys available in your repository folder.");
        return;
    }

    wizardStack->setCurrentIndex(1);
    terminalLogView->append("<span style='color: #3498db;'>[INFO] Requesting secure authorization escalation layer...</span>");

    QString selectedKeyBase = keySelectorComboBox->currentText();
    QString privKey = QString("%1/%2.priv").arg(sandboxKeyDir, selectedKeyBase);
    QString pubCert = QString("%1/%2.der").arg(sandboxKeyDir, selectedKeyBase);

    terminalLogView->append(QString("<span style='color: #95a5a6;'>[EXEC] Selected signing token: %1.priv</span>").arg(selectedKeyBase));

    QProcess *signProcess = new QProcess(this);
    QStringList args;
    args << "sbsign" << "--key" << privKey << "--cert" << pubCert << "--output" << targetKernelPath << targetKernelPath;

    connect(signProcess, &QProcess::finished, this, [this, signProcess](int exitCode, QProcess::ExitStatus status) {
        if (status == QProcess::NormalExit && exitCode == 0) {
            terminalLogView->append("<br><b style='color: #2ecc71;'>[SUCCESS] Secure Boot signature block injected cleanly into binary image.</b>");
            btnProceedToAutomation->setEnabled(true);
        } else {
            QString err = QString::fromUtf8(signProcess->readAllStandardError());
            terminalLogView->append(QString("<br><b style='color: #e74c3c;'>[FAILURE] sbsign execution failed:</b>\n%1").arg(err));
        }
        signProcess->deleteLater();
    });

    signProcess->start("pkexec", args);
}

void MokSigningWizard::finalizeAutomationDeployment()
{
    if (!chkInstallSystemHook->isChecked()) {
        this->accept();
        return;
    }

    QString selectedKeyBase = keySelectorComboBox->currentText();

    QString scriptContents =
    "#!/bin/bash\n"
    "VERSION=\"$1\"\n"
    "KERNEL_IMAGE=\"$2\"\n"
    "KEY_DIR=\"" + sandboxKeyDir + "\"\n"
    "if [ -z \"$KERNEL_IMAGE\" ]; then KERNEL_IMAGE=\"/boot/vmlinuz-$VERSION\"; fi\n"
    "if [ -f \"$KEY_DIR/" + selectedKeyBase + ".priv\" ] && [ -f \"$KERNEL_IMAGE\" ]; then\n"
    "    sbsign --key \"$KEY_DIR/" + selectedKeyBase + ".priv\" --cert \"$KEY_DIR/" + selectedKeyBase + ".der\" --output \"$KERNEL_IMAGE\" \"$KERNEL_IMAGE\"\n"
    "fi\n";

        QString tempPath = QDir::tempPath() + "/99-secureboot-manager";
        QFile file(tempPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write(scriptContents.toUtf8());
            file.close();
        }

        // FIX: Completely dropped shell execution string parsing pipelines.
        // We execute standard file operations natively as isolated system subprocess commands.
        QProcess *deployProcess = new QProcess(this);
        QString destinationTarget = "/etc/kernel/postinst.d/99-secureboot-manager";

        connect(deployProcess, &QProcess::finished, this, [this, deployProcess, destinationTarget](int exitCode, QProcess::ExitStatus status) {
            if (status == QProcess::NormalExit && exitCode == 0) {
                // Execution Part 2: Securely alter permissions explicitly without a shell command line string payload wrapper
                QProcess chmodProcess;
                chmodProcess.start("pkexec", QStringList() << "chmod" << "+x" << destinationTarget);
                chmodProcess.waitForFinished(2000);

                QMessageBox::information(this, "Success", "Automatic kernel upgrade hooks deployed successfully!");
            } else {
                QMessageBox::warning(this, "Automation Failure", "Failed to register automated hooks to /etc/kernel/.");
            }
            deployProcess->deleteLater();
            this->accept();
        });

        deployProcess->start("pkexec", QStringList() << "cp" << tempPath << destinationTarget);
}

void MokSigningWizard::executeSigningFlow(const QString &kernelVersion, QWidget *parent)
{
    MokSigningWizard wizard(kernelVersion, parent);
    wizard.exec();
}
