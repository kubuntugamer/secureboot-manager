#include "moksignerpage.h"
#include "mokuisigner.h"
#include "abstractdiscoveryengine.h"
#include "debiandiscoveryengine.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <QProcess>
#include <QApplication>
#include <QMessageBox>
#include <QTextBrowser>

MokSignerPage::MokSignerPage(QWidget *parent) : QWidget(parent)
{
    // 🛠️ NO MORE GHOST BARS: Removed the placeholder setup call that loaded empty lists on boot.
    // The panel now initializes clean and draws perfectly once the discovery backend completes.
    if (QFile::exists("/usr/bin/dpkg-query")) {
        discoveryEngine = new DebianDiscoveryEngine(this);
    } else {
        return;
    }

    connect(discoveryEngine, &AbstractDiscoveryEngine::discoveryFinished, this, [=](QStringList uNames, QStringList uPaths, QStringList sNames) {
        QList<QWidget*> lingeringChildren = this->findChildren<QWidget*>();
        for (QWidget* child : lingeringChildren) {
            child->setParent(nullptr);
            child->deleteLater();
        }

        if (this->layout()) {
            QLayoutItem *item;
            while ((item = this->layout()->takeAt(0)) != nullptr) {
                if (item->widget()) {
                    item->widget()->setParent(nullptr);
                    item->widget()->deleteLater();
                }
                delete item;
            }
            delete this->layout();
        }

        MokUiSigner::setupSigningPage(this, editSignTargetPath, editSignKeyPath, btnBrowseBinary, btnExecuteSignature, uNames, uPaths, sNames);
        setupExecutionHook();
    });

    QTimer::singleShot(50, this, &MokSignerPage::triggerScan);
}

void MokSignerPage::triggerScan()
{
    if (discoveryEngine) {
        discoveryEngine->discoverKernels();
    }
}

void MokSignerPage::setupExecutionHook()
{
    if (!btnExecuteSignature) return;

    btnExecuteSignature->disconnect();
    connect(btnExecuteSignature, &QPushButton::clicked, this, [this]() {
        QString targetBinary = this->property("selectedKernelPath").toString();
        QString keyAsset = editSignKeyPath ? editSignKeyPath->text().trimmed() : "";

        if (targetBinary.isEmpty() || keyAsset.isEmpty()) {
            QMessageBox::warning(this, "Selection Missing", "Please select a staged kernel card to sign first.");
            return;
        }

        QTextBrowser *logTerminal = this->findChild<QTextBrowser*>("signerLogTerminal");
        if (logTerminal) {
            logTerminal->clear();
            logTerminal->append("🚀 [SYSTEM INIT]: Launching asynchronous Secure Boot signing pipeline...");
            logTerminal->append(QString("📂 [TARGET BINARY]: %1").arg(targetBinary));
        }

        QString fullyResolvedKey = keyAsset;
        if (fullyResolvedKey.startsWith("~")) {
            fullyResolvedKey.replace(0, 1, QDir::homePath());
        }

        QString certAsset = fullyResolvedKey;
        if (certAsset.endsWith(".priv")) {
            certAsset.replace(".priv", ".pem");
        } else if (certAsset.endsWith(".key")) {
            certAsset.replace(".key", ".pem");
        }

        QFileInfo kernelInfo(targetBinary);
        QString kernelVersion = kernelInfo.fileName();
        if (kernelVersion.startsWith("vmlinuz-")) {
            kernelVersion = kernelVersion.mid(8);
        }

        btnExecuteSignature->setEnabled(false);
        btnExecuteSignature->setText("⏳ Injecting Cryptographic Secure Boot Signatures... Please Wait.");
        btnExecuteSignature->setStyleSheet(
            "QPushButton {"
            "  background-color: #2c3e50; color: #7f8c8d; border: 1px solid #34495e;"
            "  font-weight: bold; padding: 12px; border-radius: 4px; font-size: 11px;"
            "}"
        );
        qApp->processEvents();

        QProcess *sbSignWorker = new QProcess(qApp);
        QString modulesPath = QString("/lib/modules/%1").arg(kernelVersion);
        QDir modulesDir(modulesPath);

        if (logTerminal) {
            logTerminal->append(QString("🔑 [KEY RESOLVED]: %1").arg(fullyResolvedKey));
            logTerminal->append(QString("📜 [CERT RESOLVED]: %1").arg(certAsset));
        }

        QString scriptPayload = QString(
            "echo '📡 [KERNEL]: Injecting signature headers directly into binary core...'; sbsign --key '%1' --cert '%2' --output '%3' '%4'"
        ).arg(fullyResolvedKey, certAsset, targetBinary, targetBinary);

        if (modulesDir.exists()) {
            scriptPayload += QString(
                " && echo '📡 [MODULES]: Traversing driver module listings inside %1...';"
                " find '%1' -type f \\( -name '*.ko' -o -name '*.ko.xz' -o -name '*.ko.zst' \\) "
                "-exec sh -c 'echo \"✍️ [SIGNING MODULE]: {}\" && sbsign --key \"%2\" --cert \"%3\" --output \"{}\" \"{}\"' \\;"
            ).arg(modulesPath, fullyResolvedKey, certAsset);
        }

        connect(sbSignWorker, &QProcess::readyReadStandardOutput, qApp, [sbSignWorker, logTerminal]() {
            if (!logTerminal) return;
            QString stdOutputLines = QString::fromUtf8(sbSignWorker->readAllStandardOutput()).trimmed();
            if (!stdOutputLines.isEmpty()) {
                logTerminal->append(stdOutputLines);
            }
        });

        connect(sbSignWorker, &QProcess::readyReadStandardError, qApp, [sbSignWorker, logTerminal]() {
            if (!logTerminal) return;
            QString errOutputLines = QString::fromUtf8(sbSignWorker->readAllStandardError()).trimmed();
            if (!errOutputLines.isEmpty()) {
                logTerminal->append("<font color='#e74c3c'>⚠️ [SHELL ERROR]: " + errOutputLines + "</font>");
            }
        });

        connect(sbSignWorker, &QProcess::finished, qApp, [this, sbSignWorker, logTerminal](int exitCode, QProcess::ExitStatus status) {
            if (btnExecuteSignature) {
                btnExecuteSignature->setEnabled(true);
                btnExecuteSignature->setText("Inject Secure Boot Signature");
                btnExecuteSignature->setStyleSheet(
                    "QPushButton {"
                    "  background-color: #2a3b4d; color: #3daee9; border: 1px solid #3daee9;"
                    "  font-weight: bold; padding: 12px; border-radius: 4px; font-size: 11px;"
                    "}"
                    "QPushButton:hover { background-color: #3daee9; color: #ffffff; }"
                );
            }

            if (status == QProcess::NormalExit && exitCode == 0) {
                if (logTerminal) {
                    logTerminal->append("<font color='#1dd1a1'>✨ [SUCCESS]: Secure Boot signature injection finished with code 0! Repainting dashboard grid...</font>");
                }
                QMessageBox::information(this, "Signing Successful", "Secure Boot signatures successfully injected into your kernel assets.");
            } else {
                if (logTerminal) {
                    logTerminal->append("<font color='#e74c3c'>❌ [FATAL ERROR]: The execution subshell rolled back mid-process.</font>");
                }
                QMessageBox::critical(this, "Signing Injection Failed", "The sbsign execution loop returned an unhandled terminal error.");
            }

            if (this && this->isVisible()) {
                QTimer::singleShot(200, this, &MokSignerPage::triggerScan);
            }
            sbSignWorker->deleteLater();
        });

        sbSignWorker->start("pkexec", QStringList() << "sh" << "-c" << scriptPayload);
    });
}
