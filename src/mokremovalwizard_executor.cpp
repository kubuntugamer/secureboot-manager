#include "mokremovalwizard.h"

// 🎨 QT UI CORE COMPONENTS: Resolves incomplete type compilation errors
#include <QLineEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QMessageBox>
#include <QStackedWidget>

// System & Process Utilities
#include <QProcess>
#include <QFile>
#include <QRegularExpression>
#include <QTimer>
#include <QDir>

void launchBlockadeRebootOverlay(QWidget *parent);

void MokRemovalWizard::handleRemediationPipeline()
{
    if (affectedKernels.isEmpty()) return;

    btnStartRemediation->setEnabled(false);
    btnCancelRemediation->setEnabled(false);
    logTerminal->clear();
    logTerminal->append("<span style='color: #3498db;'>[INFO] Requesting admin escalation proxy...</span>");

    processingKernelIndex = 0;
    unsignNextKernel();
}

void MokRemovalWizard::unsignNextKernel()
{
    if (processingKernelIndex >= affectedKernels.size()) {
        logTerminal->append("<br><b style='color: #2ecc71;'>✅ Success: All kernel dependencies have been successfully unsigned.</b>");
        logTerminal->append("<span style='color: #3498db;'>Advancing to Phase 2: Firmware Deletion configuration...</span>");

        QTimer::singleShot(1200, this, [this]() { wizardStack->setCurrentIndex(1); });
        return;
    }

    QString currentKernel = affectedKernels[processingKernelIndex];
    logTerminal->append(QString("🔧 Unsigning target boundary: %1").arg(currentKernel));

    QProcess *stripProcess = new QProcess(this);

    // Trap underlying engine channel errors (e.g. system binary crashes or permissions)
    connect(stripProcess, &QProcess::errorOccurred, this, [this, currentKernel](QProcess::ProcessError error) {
        logTerminal->append(QString("<b style='color: #e74c3c;'>[CRITICAL] Local execution loop fault (%1) targeting:</b> %2")
        .arg(error).arg(currentKernel));
        btnCancelRemediation->setEnabled(true);
    });

    connect(stripProcess, &QProcess::finished, this, [this, stripProcess, currentKernel](int exitCode, QProcess::ExitStatus status) {
        if (status == QProcess::NormalExit && exitCode == 0) {
            logTerminal->append(QString("<span style='color: #2ecc71;'>[CLEARED] Removed MOK signature block from: %1</span>").arg(currentKernel));
            processingKernelIndex++;
            stripProcess->deleteLater();
            unsignNextKernel();
        } else {
            QString errMsg = QString::fromUtf8(stripProcess->readAllStandardError()).trimmed();
            if (errMsg.isEmpty()) {
                errMsg = QString::fromUtf8(stripProcess->readAllStandardOutput()).trimmed();
            }

            // Provide targeted user assistance for dismissed Polkit escalation boxes
            if (exitCode == 127 || exitCode == 126) {
                logTerminal->append("<b style='color: #ff9f43;'>[AUTH DENIED] Administrative elevation was bypassed or rejected by the local user.</b>");
            } else {
                logTerminal->append(QString("<b style='color: #e74c3c;'>[FATAL ERROR] Failed to unsign kernel profile:</b>\n%1").arg(errMsg.isEmpty() ? "Unknown sbattach sub-process failure" : errMsg));
            }
            btnCancelRemediation->setEnabled(true);
        }
    });

    stripProcess->start("pkexec", QStringList() << "sbattach" << "--remove" << currentKernel);
}

void MokRemovalWizard::executeMokNVRAMPurge()
{
    QString pass = editResetPassword->text();
    QString confirm = editResetConfirm->text();

    // =========================================================================
    // 🔒 PIPELINE EXECUTION VERIFICATION
    // ASSOCIATED CODE FILES: src/mokremovalwizard_init.cpp, src/mokremovalwizard_executor.cpp
    // ROADMAP STEP: Keep barriers low. Rely entirely on Step 2 pre-flight verification checks.
    // =========================================================================
    if (pass.isEmpty() || pass != confirm) {
        return; // Silent bypass protection. Validation feedback is already handled by the UI fields.
    }

    if (QMessageBox::question(this, "🔄 FINAL SECURITY CONFIRMATION",
        "Are you absolutely sure you want to write this deletion request into the NVRAM queue?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) return;

    // =========================================================================
    // 🔓 SOVEREIGNTY SECURED: BREAK GLOBAL WIDGET LOCKOUT
    // ASSOCIATED CODE FILES: src/mokremovalwizard_executor.cpp
    // ROADMAP STEP: Target the action button instead of disabling the entire app window
    // =========================================================================
    btnExecutePurge->setEnabled(false);
    btnExecutePurge->setText("⌛ Committing request to NVRAM...");

    QString targetDir = QDir::homePath() + "/secureboot-manager-keys";
    QString baseName = targetKeyName;
    baseName.replace(QRegularExpression("[^a-zA-Z0-9_\\-]"), "_");
    if (baseName.isEmpty()) baseName = "custom_mok_key";

    QString certPath = QString("%1/%2.der").arg(targetDir, baseName);

    // Structure arguments properly so Polkit hands the token parsing arrays off cleanly to mokutil
    QStringList runArgs;
    runArgs << "mokutil";
    if (QFile::exists(certPath)) {
        runArgs << "--delete" << certPath;
    } else {
        runArgs << "--reset";
    }

    QProcess *purgeProcess = new QProcess(this);

    connect(purgeProcess, &QProcess::started, this, [purgeProcess, pass]() {
        purgeProcess->write(pass.toUtf8() + "\n");
        purgeProcess->write(pass.toUtf8() + "\n");
    });

    connect(purgeProcess, &QProcess::finished, this, [this, purgeProcess](int exitCode, QProcess::ExitStatus status) {
        // Restore interaction controls on the trigger element if process fails out
        btnExecutePurge->setEnabled(true);
        btnExecutePurge->setText("🗑️ Commit Key Removal Request");

        if (status == QProcess::NormalExit && exitCode == 0) {
            this->accept();
            launchBlockadeRebootOverlay(this->parentWidget());
        } else {
            QString errorMsg = QString::fromUtf8(purgeProcess->readAllStandardError()).trimmed();
            if (errorMsg.isEmpty()) {
                errorMsg = QString::fromUtf8(purgeProcess->readAllStandardOutput()).trimmed();
            }

            if (exitCode == 127 || exitCode == 126) {
                QMessageBox::critical(this, "Authorization Required", "MOK modifications require authentication validation to access NVRAM lanes.");
            } else {
                QMessageBox::critical(this, "Execution Error", "mokutil deletion registration failed:\n" + (errorMsg.isEmpty() ? "Unknown structural error" : errorMsg));
            }
        }
        purgeProcess->deleteLater();
    });

    purgeProcess->start("pkexec", runArgs);
}
