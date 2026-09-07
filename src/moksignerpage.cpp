#include "moksignerpage.h"
#include "mokuisigner.h"
#include "abstractdiscoveryengine.h"
#include "debiandiscoveryengine.h"
#include "mokautomationbackend.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
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
#include <QRegularExpression>

MokSignerPage::MokSignerPage(QWidget *parent) : QWidget(parent)
{
    // ⚙️ SUBMODULE GROUP 3: Instantiate discovery components
    if (QFile::exists(QStringLiteral("/usr/bin/dpkg-query"))) {
        discoveryEngine = new DebianDiscoveryEngine(this);
    } else {
        return;
    }

    // 🎨 BASELINE VIEWS: Paints the secure landing frame panel
    QVBoxLayout *initialLayout = new QVBoxLayout(this);
    initialLayout->setContentsMargins(30, 25, 30, 25);
    initialLayout->setSpacing(15);

    QLabel *noticeHeader = new QLabel(QStringLiteral("🔒 Secure UEFI Boundary Gate"), this);
    noticeHeader->setStyleSheet(QStringLiteral("font-size: 16px; font-weight: bold; color: #ffffff;"));
    initialLayout->addWidget(noticeHeader);

    QLabel *noticeDesc = new QLabel(QStringLiteral("To read root-locked system binary kernels and verify cryptographic signatures\nwithout folder attribute manipulation, this utility requires a single elevated authentication pass."), this);
    noticeDesc->setStyleSheet(QStringLiteral("color: #a4b0be; font-size: 11px; font-family: monospace;"));
    initialLayout->addWidget(noticeDesc);

    QPushButton *btnInitScan = new QPushButton(QStringLiteral("🔍 Authenticate and Initialize System Scanner"), this);
    btnInitScan->setStyleSheet(
        QStringLiteral(
            "QPushButton {"
            "  background-color: #2c3e50; color: #3daee9; border: 1px solid #3daee9;"
            "  font-weight: bold; padding: 14px; border-radius: 4px; font-size: 12px;"
            "}"
            "QPushButton:hover { background-color: #3daee9; color: #ffffff; }"
        )
    );
    initialLayout->addWidget(btnInitScan);
    initialLayout->addStretch();
    // 📡 DATA SYNCHRONIZATION PIPELINE: Handles drawing active card entries when elevated scans complete
    connect(discoveryEngine, &AbstractDiscoveryEngine::discoveryFinished, this, [=](QStringList uNames, QStringList uPaths, QStringList sNames) {
        QString existingLogText;
        if (auto oldLog = this->findChild<QTextBrowser*>(QStringLiteral("signerLogTerminal"))) {
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
        QDir bootDir(QStringLiteral("/boot"));
        for (const QString &signedName : sNames) {
            resolvedSignedPaths.append(bootDir.filePath(QStringLiteral("vmlinuz-") + signedName));
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

            QPushButton *btnExecuteUnsign = new QPushButton(QStringLiteral("Strip / Unsign Selected Binary"), this);
            btnExecuteUnsign->setObjectName(QStringLiteral("btnExecuteUnsign"));
            btnExecuteUnsign->setStyleSheet(
                QStringLiteral(
                    "QPushButton {"
                    "  background-color: #3d2222; color: #ff6b6b; border: 1px solid #ff6b6b;"
                    "  font-weight: bold; padding: 12px; border-radius: 4px; font-size: 11px;"
                    "}"
                    "QPushButton:hover { background-color: #ff6b6b; color: #ffffff; }"
                )
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
            if (auto newLog = this->findChild<QTextBrowser*>(QStringLiteral("signerLogTerminal"))) {
                newLog->setPlainText(existingLogText);
            }
        }

        // ⚡ HOOKS LINKAGE: Rebind signature and stripping execution paths back onto the structural buttons
        setupExecutionHook();
    });

    // 🔒 SINGLE ELEVATED INITIALIZATION HOOK: Intercepts clicks to fire privilege pass sequence
    connect(btnInitScan, &QPushButton::clicked, this, [this, btnInitScan]() {
        btnInitScan->setEnabled(false);
        btnInitScan->setText(QStringLiteral("⏳ Awaiting System Privilege Authentication..."));
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
        QString keyAsset = editSignKeyPath ? editSignKeyPath->text().trimmed() : QString();

        if (targetBinary.isEmpty() || keyAsset.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("Selection Missing"), QStringLiteral("Please select a staged kernel card to sign first."));
            return;
        }

        QString fullyResolvedKey = keyAsset;
        if (fullyResolvedKey.startsWith(QStringLiteral("~"))) {
            fullyResolvedKey.replace(0, 1, QDir::homePath());
        }

        // Auto-resolve certificate mapping extensions securely
        QString certAsset = fullyResolvedKey;
        if (certAsset.endsWith(QStringLiteral(".priv"))) {
            certAsset.replace(QStringLiteral(".priv"), QStringLiteral(".pem"));
        } else if (certAsset.endsWith(QStringLiteral(".key"))) {
            certAsset.replace(QStringLiteral(".key"), QStringLiteral(".pem"));
        }

        // Handle path validation for certificates fallback (.der conversion fallback)
        if (!QFile::exists(certAsset)) {
            QString derFallback = fullyResolvedKey;
            derFallback.replace(QRegularExpression(QStringLiteral("\\.(key|priv)$")), QStringLiteral(".der"));
            if (QFile::exists(derFallback)) {
                certAsset = derFallback;
            }
        }

        QTextBrowser *logTerminal = this->findChild<QTextBrowser*>(QStringLiteral("signerLogTerminal"));
        if (logTerminal) {
            logTerminal->clear();
            logTerminal->append(QStringLiteral("🚀 [SYSTEM INIT]: Launching structural signing pipeline..."));
            logTerminal->append(QString("📂 [TARGET BINARY]: %1").arg(targetBinary));
            logTerminal->append(QString("🔑 [KEY ASSET]: %1").arg(fullyResolvedKey));
            logTerminal->append(QString("📜 [CERT ASSET]: %1").arg(certAsset));
        }

        QPushButton *btnExecuteUnsign = this->findChild<QPushButton*>(QStringLiteral("btnExecuteUnsign"));
        btnExecuteSignature->setEnabled(false);
        if (btnExecuteUnsign) btnExecuteUnsign->setEnabled(false);
        btnExecuteSignature->setText(QStringLiteral("⏳ Processing Atomic Signature Pipeline... Please Wait."));
        qApp->processEvents();

        // Rule 1 Compliance: Run directly via token arrays, banning raw "sh -c" scripts entirely
        QProcess *sbSignWorker = new QProcess(qApp);
        QString program = QStringLiteral("/usr/bin/pkexec");

        QStringList arguments;
        arguments << QStringLiteral("sbsign")
        << QStringLiteral("--key") << fullyResolvedKey
        << QStringLiteral("--cert") << certAsset
        << QStringLiteral("--output") << targetBinary
        << targetBinary;

        connect(sbSignWorker, &QProcess::readyReadStandardOutput, qApp, [sbSignWorker, logTerminal]() {
            if (!logTerminal) return;
            QString lines = QString::fromUtf8(sbSignWorker->readAllStandardOutput()).trimmed();
            if (!lines.isEmpty()) logTerminal->append(lines);
        });

            connect(sbSignWorker, &QProcess::readyReadStandardError, qApp, [sbSignWorker, logTerminal]() {
                if (!logTerminal) return;
                QString lines = QString::fromUtf8(sbSignWorker->readAllStandardError()).trimmed();
                if (!lines.isEmpty()) logTerminal->append(QStringLiteral("<font color='#e74c3c'>⚠️ ") + lines + QStringLiteral("</font>"));
            });

                // FIXED CONNECT PARAMS: Cleared trailing context tokens to prevent template evaluation limits
                connect(sbSignWorker, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
                        [this, sbSignWorker, btnExecuteUnsign, logTerminal](int exitCode, QProcess::ExitStatus status) {

                            if (btnExecuteSignature) {
                                btnExecuteSignature->setEnabled(true);
                                btnExecuteSignature->setText(QStringLiteral("Inject Secure Boot Signature"));
                            }
                            if (btnExecuteUnsign) btnExecuteUnsign->setEnabled(true);

                            // Read log history safely to identify internal errors
                            QString logHistory = logTerminal ? logTerminal->toPlainText() : QString();
                            bool hasInternalErrors = logHistory.contains(QStringLiteral("error:")) ||
                            logHistory.contains(QStringLiteral("Can't load")) ||
                            logHistory.contains(QStringLiteral("No such file"));

                            if (status == QProcess::NormalExit && exitCode == 0 && !hasInternalErrors) {
                                if (logTerminal) {
                                    logTerminal->append(QStringLiteral("<font color='#1dd1a1'>✨ [SUCCESS]: Signature token clean-injected successfully!</font>"));
                                }
                                QMessageBox::information(this, QStringLiteral("Signing Successful"), QStringLiteral("Secure Boot signatures successfully clean-injected."));
                            } else {
                                if (logTerminal) {
                                    logTerminal->append(QStringLiteral("<font color='#e74c3c'>❌ [FAILURE]: Pipeline aborted due to underlying tool runtime failure.</font>"));
                                }
                                QMessageBox::critical(this, QStringLiteral("Signing Injection Failed"), QStringLiteral("The execution layer reported errors. Please check the logs above to ensure your key and certificate configurations are valid."));
                            }

                            sbSignWorker->deleteLater();

                            // Sanitize cache properties and refresh list widget elements
                            this->setProperty("selectedKernelPath", QString());
                            QTimer::singleShot(400, this, &MokSignerPage::triggerScan);
                        });

                sbSignWorker->start(program, arguments);
    });
    // ✂️ SUBMODULE GROUP 4 REVERSE INITIATION: Signature stripping pipeline connection
    QPushButton *btnExecuteUnsign = this->findChild<QPushButton*>(QStringLiteral("btnExecuteUnsign"));
    if (btnExecuteUnsign) {
        btnExecuteUnsign->disconnect();
        connect(btnExecuteUnsign, &QPushButton::clicked, this, [this]() {
            QString targetBinary = this->property("selectedKernelPath").toString();

            if (targetBinary.isEmpty()) {
                QMessageBox::warning(this, QStringLiteral("Selection Missing"), QStringLiteral("Please select a target kernel binary from the verified list to unsign first."));
                return;
            }

            QPushButton *btnExecuteUnsign = this->findChild<QPushButton*>(QStringLiteral("btnExecuteUnsign"));
            QTextBrowser *logTerminal = this->findChild<QTextBrowser*>(QStringLiteral("signerLogTerminal"));

            if (logTerminal) {
                logTerminal->clear();
                logTerminal->append(QStringLiteral("⚠️ [SYSTEM INIT]: Launching kernel signature removal pipeline..."));
                logTerminal->append(QString("📂 [TARGET BINARY]: %1").arg(targetBinary));
            }

            // Lock controls during execution to block double clicks
            if (btnExecuteSignature) btnExecuteSignature->setEnabled(false);
            if (btnExecuteUnsign) {
                btnExecuteUnsign->setEnabled(false);
                btnExecuteUnsign->setText(QStringLiteral("⏳ Stripping Signature..."));
            }
            qApp->processEvents();

            // Run asynchronously via standard QProcess token streams (Rule 1 & Rule 2 compliant)
            QProcess *sbUnsignWorker = new QProcess(qApp);
            QString program = QStringLiteral("/usr/bin/pkexec");

            QStringList arguments;
            arguments << QStringLiteral("sbattach") << QStringLiteral("--remove") << targetBinary;

            connect(sbUnsignWorker, &QProcess::readyReadStandardOutput, qApp, [sbUnsignWorker, logTerminal]() {
                if (!logTerminal) return;
                QString lines = QString::fromUtf8(sbUnsignWorker->readAllStandardOutput()).trimmed();
                if (!lines.isEmpty()) logTerminal->append(lines);
            });

                connect(sbUnsignWorker, &QProcess::readyReadStandardError, qApp, [sbUnsignWorker, logTerminal]() {
                    if (!logTerminal) return;
                    QString lines = QString::fromUtf8(sbUnsignWorker->readAllStandardError()).trimmed();
                    if (!lines.isEmpty()) logTerminal->append(QStringLiteral("<font color='#e74c3c'>⚠️ ") + lines + QStringLiteral("</font>"));
                });

                    // FIXED CONNECT PARAMS: Cleaned context tracking signature slots to bypass build blocks
                    connect(sbUnsignWorker, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
                            [this, sbUnsignWorker, btnExecuteUnsign, logTerminal](int exitCode, QProcess::ExitStatus status) {

                                if (btnExecuteSignature) btnExecuteSignature->setEnabled(true);
                                if (btnExecuteUnsign) {
                                    btnExecuteUnsign->setEnabled(true);
                                    btnExecuteUnsign->setText(QStringLiteral("Strip / Unsign Selected Binary"));
                                }

                                if (status == QProcess::NormalExit && exitCode == 0) {
                                    if (logTerminal) {
                                        logTerminal->append(QStringLiteral("<font color='#1dd1a1'>✨ [SUCCESS]: Signature block stripped clean from target image binary.</font>"));
                                    }
                                    QMessageBox::information(this, QStringLiteral("Unsign Successful"), QStringLiteral("Secure Boot signature successfully removed from the kernel binary."));
                                } else {
                                    if (logTerminal) {
                                        logTerminal->append(QStringLiteral("<font color='#e74c3c'>❌ [FAILURE]: Signature separation execution aborted.</font>"));
                                    }
                                    QMessageBox::critical(this, QStringLiteral("Unsign Failed"), QStringLiteral("The signature removal process encountered an error."));
                                }

                                sbUnsignWorker->deleteLater();

                                // Clean tracking metrics references and trigger scan redraw wait buffer
                                this->setProperty("selectedKernelPath", QString());
                                QTimer::singleShot(400, this, &MokSignerPage::triggerScan);
                            });

                    sbUnsignWorker->start(program, arguments);
        });
    }
}
