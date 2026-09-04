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

MokSignerPage::MokSignerPage(QWidget *parent) : QWidget(parent)
{
    MokUiSigner::setupSigningPage(this,
                                  editSignTargetPath,
                                  editSignKeyPath,
                                  btnBrowseBinary,
                                  btnExecuteSignature,
                                  QStringList(),
                                  QStringList(),
                                  QStringList());
    setupExecutionHook();

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

        QString fullyResolvedKey = keyAsset;
        if (fullyResolvedKey.startsWith("~")) {
            fullyResolvedKey.replace(0, 1, QDir::homePath());
        }

        // 🛠️ FIXED EXTENSION RE-MAPPER: Safely maps your public certificate pointer targets straight to MOK.pem
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

        // 🔄 VISUAL PROGRESS INDICATOR: Lock down the interface and switch button to active execution state
        btnExecuteSignature->setEnabled(false);
        btnExecuteSignature->setText("⏳ Injecting Cryptographic Secure Boot Signatures... Please Wait.");
        btnExecuteSignature->setStyleSheet(
            "QPushButton {"
            "  background-color: #2c3e50; color: #7f8c8d; border: 1px solid #34495e;"
            "  font-weight: bold; padding: 12px; border-radius: 4px; font-size: 11px;"
            "}"
        );
        qApp->processEvents(); // Force layout refresh to reflect state immediately

        QProcess *sbSignWorker = new QProcess(qApp);
        QString modulesPath = QString("/lib/modules/%1").arg(kernelVersion);
        QDir modulesDir(modulesPath);

        QString scriptPayload = QString(
            "sbsign --key '%1' --cert '%2' --output '%3' '%4'"
        ).arg(fullyResolvedKey, certAsset, targetBinary, targetBinary);

        if (modulesDir.exists()) {
            scriptPayload += QString(
                " && find '%1' -type f \\( -name '*.ko' -o -name '*.ko.xz' -o -name '*.ko.zst' \\) "
                "-exec sbsign --key '%2' --cert '%3' --output {} {} \\;"
            ).arg(modulesPath, fullyResolvedKey, certAsset);
        }

        connect(sbSignWorker, &QProcess::finished, qApp, [this, sbSignWorker](int exitCode, QProcess::ExitStatus status) {
            QString errorDetails = QString::fromUtf8(sbSignWorker->readAllStandardError()).trimmed();
            QString standardDetails = QString::fromUtf8(sbSignWorker->readAllStandardOutput()).trimmed();

            // 🔄 VISUAL RESTORATION: Re-enable the layout button immediately upon worker thread termination
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
                QMessageBox::information(this, "Signing Successful",
                                         QString("Secure Boot signatures successfully injected into:\n\n📌 %1\n\nAll linked kernel modules have been validated.").arg(this->property("selectedKernelPath").toString()));
            } else {
                QString errorMsg = errorDetails.isEmpty() ? standardDetails : errorDetails;
                if (errorMsg.isEmpty()) errorMsg = "System privilege token rejected or command terminated by user.";

                QMessageBox::critical(this, "Signing Injection Failed",
                                      "The sbsign execution loop returned an error wrapper:\n\n" + errorMsg);
            }

            if (this && this->isVisible()) {
                QTimer::singleShot(200, this, &MokSignerPage::triggerScan);
            }
            sbSignWorker->deleteLater();
        });

        sbSignWorker->start("pkexec", QStringList() << "sh" << "-c" << scriptPayload);
    });
}
