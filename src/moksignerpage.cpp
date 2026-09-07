#include "moksignerpage.h"
#include "mokuisigner.h"
#include "abstractdiscoveryengine.h"
#include "debiandiscoveryengine.h"
#include "mokautomationbackend.h"
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
#include <QLabel>

MokSignerPage::MokSignerPage(QWidget *parent) : QWidget(parent)
{
    // ⚙️ SUBMODULE GROUP 3: Instantiate our underlying platform discovery framework components
    if (QFile::exists("/usr/bin/dpkg-query")) {
        discoveryEngine = new DebianDiscoveryEngine(this);
    } else {
        return;
    }

    // 🎨 USER-INITIATED BASELINE VIEWS: Paints a clean boundary lock panel on boot
    QVBoxLayout *initialLayout = new QVBoxLayout(this);
    initialLayout->setContentsMargins(30, 25, 30, 25);
    initialLayout->setSpacing(15);

    QLabel *noticeHeader = new QLabel("🔒 Secure UEFI Boundary Gate", this);
    noticeHeader->setStyleSheet("font-size: 16px; font-weight: bold; color: #ffffff;");
    initialLayout->addWidget(noticeHeader);

    QLabel *noticeDesc = new QLabel("To read root-locked system binary kernels and verify cryptographic signatures\nwithout folder attribute manipulation, this utility requires a single elevated authentication pass.", this);
    noticeDesc->setStyleSheet("color: #a4b0be; font-size: 11px; font-family: monospace;");
    initialLayout->addWidget(noticeDesc);

    QPushButton *btnInitScan = new QPushButton("🔍 Authenticate and Initialize System Scanner", this);
    btnInitScan->setStyleSheet(
        "QPushButton {"
        "  background-color: #2c3e50; color: #3daee9; border: 1px solid #3daee9;"
        "  font-weight: bold; padding: 14px; border-radius: 4px; font-size: 12px;"
        "}"
        "QPushButton:hover { background-color: #3daee9; color: #ffffff; }"
    );
    initialLayout->addWidget(btnInitScan);
    initialLayout->addStretch();

    // 📡 DATA SYNCHRONIZATION PIPELINE: Handles drawing active card entries when elevated scans complete
    connect(discoveryEngine, &AbstractDiscoveryEngine::discoveryFinished, this, [=](QStringList uNames, QStringList uPaths, QStringList sNames) {
        QString existingLogText;
        if (auto oldLog = this->findChild<QTextBrowser*>("signerLogTerminal")) {
            existingLogText = oldLog->toPlainText();
        }

        // 🧹 RENDERING PURGE: Wipe out old widgets cleanly before drawing active drive metrics
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

        // 💎 FIXED PATH VECTOR MAPPING: Map accurate boot paths for verified images using live metrics
        QStringList resolvedSignedPaths;
        QDir bootDir("/boot");
        for (const QString &signedName : sNames) {
            resolvedSignedPaths.append(bootDir.filePath("vmlinuz-" + signedName));
        }

        // 🎨 SUBMODULE GROUP 2 CALL: Paint the dynamic column layouts based on genuine root data vectors
        MokUiSigner::setupSigningPage(this, editSignTargetPath, editSignKeyPath, btnBrowseBinary, btnExecuteSignature,
                                      uNames, uPaths, sNames, resolvedSignedPaths);

        if (auto mainLayout = qobject_cast<QVBoxLayout*>(this->layout())) {
            int idx = mainLayout->indexOf(btnExecuteSignature);
            if (idx != -1) {
                mainLayout->removeWidget(btnExecuteSignature);
            }

            QHBoxLayout *buttonLayout = new QHBoxLayout();
            buttonLayout->setSpacing(10);

            QPushButton *btnExecuteUnsign = new QPushButton("Strip / Unsign Selected Binary", this);
            btnExecuteUnsign->setObjectName("btnExecuteUnsign");
            btnExecuteUnsign->setStyleSheet(
                "QPushButton {"
                "  background-color: #3d2222; color: #ff6b6b; border: 1px solid #ff6b6b;"
                "  font-weight: bold; padding: 12px; border-radius: 4px; font-size: 11px;"
                "}"
                "QPushButton:hover { background-color: #ff6b6b; color: #ffffff; }"
            );

            buttonLayout->addWidget(btnExecuteSignature, 1);
            buttonLayout->addWidget(btnExecuteUnsign, 1);

            if (idx != -1) {
                mainLayout->insertLayout(idx, buttonLayout);
            } else {
                mainLayout->addLayout(buttonLayout);
            }
        }

        if (!existingLogText.isEmpty()) {
            if (auto newLog = this->findChild<QTextBrowser*>("signerLogTerminal")) {
                newLog->setPlainText(existingLogText);
            }
        }

        // ⚡ HOOKS LINKAGE: Rebind signature and stripping execution paths back onto the structural buttons
        setupExecutionHook();
    });

    // 🔒 SINGLE ELEVATED INITIALIZATION HOOK: Intercepts clicks to fire our privilege password verification sequence
    connect(btnInitScan, &QPushButton::clicked, this, [this, btnInitScan]() {
        btnInitScan->setEnabled(false);
        btnInitScan->setText("⏳ Awaiting System Privilege Authentication...");
        qApp->processEvents();

        // Trigger the internal partition scan thread loop
        this->triggerScan();
    });
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

    // 🔐 SUBMODULE GROUP 4 INITIATION: Secure Boot signature injection payload connection
    btnExecuteSignature->disconnect();
    connect(btnExecuteSignature, &QPushButton::clicked, this, [this]() {
        QString targetBinary = this->property("selectedKernelPath").toString();
        QString keyAsset = editSignKeyPath ? editSignKeyPath->text().trimmed() : "";

        if (targetBinary.isEmpty() || keyAsset.isEmpty()) {
            QMessageBox::warning(this, "Selection Missing", "Please select a staged kernel card to sign first.");
            return;
        }

        QFileInfo kernelInfo(targetBinary);
        QString kernelVersion = kernelInfo.fileName().mid(8);

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

        QTextBrowser *logTerminal = this->findChild<QTextBrowser*>("signerLogTerminal");
        if (logTerminal) {
            logTerminal->clear();
            logTerminal->append("🚀 [SYSTEM INIT]: Launching modular signing pipeline...");
            logTerminal->append(QString("📂 [TARGET BINARY]: %1").arg(targetBinary));
        }

        btnExecuteSignature->setEnabled(false);
        btnExecuteSignature->setText("⏳ Processing Atomic Signature Pipeline... Please Wait.");
        qApp->processEvents();

        // Invokes the script generation engine inside mokautomationbackend.cpp
        QString scriptPayload = MokAutomationBackend::generateSigningPayload(fullyResolvedKey, certAsset, targetBinary, kernelVersion);

        QProcess *sbSignWorker = new QProcess(qApp);

        connect(sbSignWorker, &QProcess::readyReadStandardOutput, qApp, [sbSignWorker, logTerminal]() {
            if (!logTerminal) return;
            QString lines = QString::fromUtf8(sbSignWorker->readAllStandardOutput()).trimmed();
            if (!lines.isEmpty()) logTerminal->append(lines);
        });

            connect(sbSignWorker, &QProcess::readyReadStandardError, qApp, [sbSignWorker, logTerminal]() {
                if (!logTerminal) return;
                QString lines = QString::fromUtf8(sbSignWorker->readAllStandardError()).trimmed();
                if (!lines.isEmpty()) logTerminal->append("<font color='#e74c3c'>⚠️ " + lines + "</font>");
            });

                connect(sbSignWorker, &QProcess::finished, qApp, [this, sbSignWorker, logTerminal](int exitCode, QProcess::ExitStatus status) {
                    if (btnExecuteSignature) {
                        btnExecuteSignature->setEnabled(true);
                        btnExecuteSignature->setText("Inject Secure Boot Signature");
                    }

                    // 🔬 DEEP ERROR INSPECTION: Scan terminal history text to catch false shell success states
                    QString logHistory = logTerminal ? logTerminal->toPlainText() : "";
                    bool hasInternalErrors = logHistory.contains("error:") ||
                    logHistory.contains("Can't load") ||
                    logHistory.contains("No such file");

                    if (status == QProcess::NormalExit && exitCode == 0 && !hasInternalErrors) {
                        if (logTerminal) {
                            logTerminal->append("<font color='#1dd1a1'>✨ [SUCCESS]: Modular signature pipeline finished with code 0!</font>");
                        }
                        QMessageBox::information(this, "Signing Successful", "Secure Boot signatures successfully clean-injected.");
                    } else {
                        if (logTerminal) {
                            logTerminal->append("<font color='#e74c3c'>❌ [FAILURE]: Pipeline aborted due to underlying tool runtime failure.</font>");
                        }
                        QMessageBox::critical(this, "Signing Injection Failed", "The execution layer reported errors. Please check the logs above to ensure your key and matching certificate (.pem) files both exist.");
                    }

                    sbSignWorker->deleteLater();

                    // 🧼 DATA CACHE SANITIZATION: Clear state tracking references cleanly
                    this->setProperty("selectedKernelPath", QString());

                    // ⏱️ REDRAW WAIT BUFFER: Pauses 400ms to allow partition edits to clear, then triggers triggerScan() to run Group 3
                    QTimer::singleShot(400, this, &MokSignerPage::triggerScan);
                });

                sbSignWorker->start("pkexec", QStringList() << "sh" << "-c" << scriptPayload);
    });

    // ✂️ SUBMODULE GROUP 4 REVERSE INITIATION: Signature stripping pipeline connection
    QPushButton *btnExecuteUnsign = this->findChild<QPushButton*>("btnExecuteUnsign");
    if (btnExecuteUnsign) {
        btnExecuteUnsign->disconnect();
        connect(btnExecuteUnsign, &QPushButton::clicked, this, [this]() {
            QString targetBinary = this->property("selectedKernelPath").toString();

            if (targetBinary.isEmpty()) {
                QMessageBox::warning(this, "Selection Missing", "Please select a target kernel binary from the verified list to unsign first.");
                return;
            }

            QTextBrowser *logTerminal = this->findChild<QTextBrowser*>("signerLogTerminal");
            if (logTerminal) {
                logTerminal->clear();
                logTerminal->append("⚠️ [SYSTEM INIT]: Launching kernel signature removal pipeline...");
                logTerminal->append(QString("📂 [TARGET BINARY]: %1").arg(targetBinary));
            }

            // Invokes the detached header stripping system tool routines inside mokuisigner.cpp
            bool success = MokUiSigner::unsignKernelBinary(targetBinary);

            if (success) {
                QMessageBox::information(this, "Unsign Successful", "Secure Boot signature successfully removed from the kernel binary.");
            } else {
                QMessageBox::critical(this, "Unsign Failed", "The signature removal process encountered an error.");
            }

            // 🧼 DATA CACHE SANITIZATION: Clear state tracking references cleanly
            this->setProperty("selectedKernelPath", QString());

            // ⏱️ REDRAW WAIT BUFFER: Pauses 400ms to allow partition edits to clear, then triggers triggerScan() to run Group 3
            QTimer::singleShot(400, this, &MokSignerPage::triggerScan);
        });
    }
}
