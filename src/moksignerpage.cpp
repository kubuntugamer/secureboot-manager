#include "moksignerpage.h"
#include "mokuisigner.h"
#include "abstractdiscoveryengine.h"
#include "debiandiscoveryengine.h"
#include "mokautomationbackend.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDir>
#include <QTimer>
#include <QProcess>
#include <QApplication>
#include <QMessageBox>
#include <QTextBrowser>
#include <QLabel>
#include <QVariant>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

MokSignerPage::MokSignerPage(QWidget *parent)
: QWidget(parent)
, m_keyComboBox(nullptr)
{
    if (QFile::exists(QStringLiteral("/usr/bin/dpkg-query"))) {
        discoveryEngine = new DebianDiscoveryEngine(this);
    } else {
        return;
    }

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

    connect(discoveryEngine, &AbstractDiscoveryEngine::discoveryFinished, this, [=](QStringList uNames, QStringList uPaths, QStringList sNames) {
        QString existingLogText;
        if (auto oldLog = this->findChild<QTextBrowser*>(QStringLiteral("signerLogTerminal"))) {
            existingLogText = oldLog->toPlainText();
        }

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

        QStringList resolvedSignedPaths;
        QDir bootDir(QStringLiteral("/boot"));
        for (const QString &signedName : sNames) {
            resolvedSignedPaths.append(bootDir.filePath(QStringLiteral("vmlinuz-") + signedName));
        }

        QLineEdit *dummyLegacyLine = nullptr;
        MokUiSigner::setupSigningPage(this, editSignTargetPath, dummyLegacyLine, btnBrowseBinary, btnExecuteSignature,
                                      uNames, uPaths, sNames, resolvedSignedPaths);

        m_keyComboBox = new QComboBox(this);
        m_keyComboBox->setMinimumWidth(350);
        m_keyComboBox->setStyleSheet(
            QStringLiteral(
                "QComboBox {"
                "  background-color: #1e272e; color: #ffffff; border: 1px solid #3daee9;"
                "  padding: 8px; border-radius: 4px; font-family: monospace;"
                "}"
                "QComboBox:disabled { background-color: #2f3542; color: #747d8c; border: 1px solid #747d8c; }"
            )
        );

        if (auto mainLayout = qobject_cast<QVBoxLayout*>(this->layout())) {
            QHBoxLayout *selectorLayout = new QHBoxLayout();
            QLabel *selectorLabel = new QLabel(QStringLiteral("🔑 Selected Secure Boot Key Profile:"), this);
            selectorLabel->setStyleSheet(QStringLiteral("color: #ffffff; font-weight: bold; font-size: 11px;"));

            selectorLayout->addWidget(selectorLabel);
            selectorLayout->addWidget(m_keyComboBox, 1);

            int runIdx = mainLayout->indexOf(btnExecuteSignature);
            if (runIdx != -1) {
                mainLayout->insertLayout(runIdx, selectorLayout);
            } else {
                mainLayout->addLayout(selectorLayout);
            }

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

        populateKeySelector();

        if (!existingLogText.isEmpty()) {
            if (auto newLog = this->findChild<QTextBrowser*>(QStringLiteral("signerLogTerminal"))) {
                newLog->setPlainText(existingLogText);
            }
        }

        setupExecutionHook();
    });

    connect(btnInitScan, &QPushButton::clicked, this, [this, btnInitScan]() {
        btnInitScan->setEnabled(false);
        btnInitScan->setText(QStringLiteral("⏳ Awaiting System Privilege Authentication..."));
        qApp->processEvents();
        this->triggerScan();
    });
}
QString MokSignerPage::extractCommonName(const QString &certPath)
{
    if (certPath.isEmpty() || !QFile::exists(certPath)) {
        return QString();
    }

    QProcess process;
    QString program = QStringLiteral("/usr/bin/openssl");
    QStringList arguments;

    // Standard x509 display configuration
    arguments << QStringLiteral("x509")
    << QStringLiteral("-in") << certPath
    << QStringLiteral("-noout")
    << QStringLiteral("-subject");

    // Force binary DER decryption format mapping explicitly for matching extensions
    if (certPath.endsWith(QStringLiteral(".der"), Qt::CaseInsensitive)) {
        arguments << QStringLiteral("-inform") << QStringLiteral("DER");
    } else {
        arguments << QStringLiteral("-inform") << QStringLiteral("PEM");
    }

    process.start(program, arguments);
    if (!process.waitForStarted(1500)) return QString();
    if (!process.waitForFinished(3000)) return QString();

    QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();

    // Capture standard x509 subject line attributes across variable styles
    QRegularExpression cnRegex(QStringLiteral("(?:CN\\s*=\\s*|\\/CN\\s*=)([^/,\n]+)"));
    QRegularExpressionMatch match = cnRegex.match(output);

    if (match.hasMatch()) {
        QString parsedName = match.captured(1).trimmed();

        // Sanitize string data by wiping raw bounding text quotes
        parsedName.remove(QStringLiteral("\""));
        parsedName.remove(QStringLiteral("'"));

        if (!parsedName.isEmpty()) {
            return parsedName;
        }
    }

    return QString();
}


void MokSignerPage::populateKeySelector()
{
    if (!m_keyComboBox) return;

    m_keyComboBox->clear();
    m_keyProfiles.clear();

    QString homePath = QDir::homePath();
    QStringList searchDirs = {
        homePath + QStringLiteral("/secureboot-manager-keys/"),
        QStringLiteral("/var/lib/shim-signed/mok/")
    };

    for (const QString &dirPath : searchDirs) {
        QDir dir(dirPath);
        if (!dir.exists()) continue;

        QStringList filters = { QStringLiteral("*.priv"), QStringLiteral("*.key") };
        QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files | QDir::Readable);

        for (const QFileInfo &fileInfo : fileList) {
            QString keyPath = fileInfo.absoluteFilePath();
            QString baseName = fileInfo.baseName();
            QString certPath = findMatchingCertificate(dirPath, baseName);

            KeyProfile profile;
            profile.privateKeyPath = keyPath;
            profile.certificatePath = certPath;
            profile.displayName = baseName;
            profile.originPath = dirPath;

            QString resolvedTitle = extractCommonName(certPath);

            if (resolvedTitle.isEmpty()) {
                resolvedTitle = baseName;
                resolvedTitle.replace(QStringLiteral("_"), QStringLiteral(" "));
                resolvedTitle.replace(QStringLiteral("-"), QStringLiteral(" "));
            }

            QString displayText = resolvedTitle;
            if (certPath.isEmpty()) {
                displayText += QStringLiteral(" ⚠️ (Missing Certificate)");
            } else {
                if (dirPath.contains(QStringLiteral("/var/lib/"))) {
                    displayText += QStringLiteral(" [System Default]");
                } else {
                    displayText += QStringLiteral(" [User Key]");
                }
            }

            m_keyProfiles.append(profile);
            m_keyComboBox->addItem(displayText, QVariant::fromValue(keyPath));
        }
    }

    if (m_keyComboBox->count() == 0) {
        m_keyComboBox->addItem(QStringLiteral("No keys found. Place profiles in ~/secureboot-manager-keys/"), QString());
        m_keyComboBox->setEnabled(false);
    } else {
        m_keyComboBox->setEnabled(true);
        for (int i = 0; i < m_keyComboBox->count(); ++i) {
            QString itemText = m_keyComboBox->itemText(i);
            if (itemText.contains(QStringLiteral("Master"), Qt::CaseInsensitive) ||
                itemText.contains(QStringLiteral("Custom"), Qt::CaseInsensitive)) {
                m_keyComboBox->setCurrentIndex(i);
            break;
                }
        }

        if (auto logNewTerminal = this->findChild<QTextBrowser*>(QStringLiteral("signerLogTerminal"))) {
            QString initialKey = m_keyComboBox->currentData().toString();
            logNewTerminal->append(QStringLiteral("💡 [SMART ASSIST]: Bound dynamic signing selector to: ") + initialKey);
        }
    }
}

QString MokSignerPage::findMatchingCertificate(const QString &dirPath, const QString &baseName)
{
    QStringList exactExtensions = { QStringLiteral(".pem"), QStringLiteral(".der"), QStringLiteral(".crt") };
    for (const QString &ext : exactExtensions) {
        QString fullCertPath = dirPath + baseName + ext;
        if (QFileInfo::exists(fullCertPath)) return fullCertPath;
    }
    return QString();
}

void MokSignerPage::triggerScan()
{
    if (discoveryEngine) discoveryEngine->discoverKernels();
}
void MokSignerPage::setupExecutionHook()
{
    if (!btnExecuteSignature) return;

    btnExecuteSignature->disconnect();
    connect(btnExecuteSignature, &QPushButton::clicked, this, [this]() {
        QString targetBinary = this->property("selectedKernelPath").toString();

        if (!m_keyComboBox || m_keyComboBox->currentIndex() == -1) {
            QMessageBox::warning(this, QStringLiteral("Selection Missing"), QStringLiteral("Please install or select a valid Secure Boot key profile first."));
            return;
        }

        QString fullyResolvedKey = m_keyComboBox->currentData().toString();
        if (targetBinary.isEmpty() || fullyResolvedKey.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("Selection Missing"), QStringLiteral("Please select a staged kernel card to sign first."));
            return;
        }

        int activeIdx = m_keyComboBox->currentIndex();
        QString certAsset;
        if (activeIdx >= 0 && activeIdx < m_keyProfiles.size()) {
            certAsset = m_keyProfiles.at(activeIdx).certificatePath;
        }

        if (certAsset.isEmpty() || !QFile::exists(certAsset)) {
            QMessageBox::critical(this, QStringLiteral("Certificate Missing"),
                                  QStringLiteral("The public validation certificate (.pem/.der/.crt) paired to this key could not be resolved."));
            return;
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

                connect(sbSignWorker, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
                        [this, sbSignWorker, btnExecuteUnsign, logTerminal](int exitCode, QProcess::ExitStatus status) {

                            if (btnExecuteSignature) {
                                btnExecuteSignature->setEnabled(true);
                                btnExecuteSignature->setText(QStringLiteral("Inject Secure Boot Signature"));
                            }
                            if (btnExecuteUnsign) btnExecuteUnsign->setEnabled(true);

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
                            this->setProperty("selectedKernelPath", QString());
                            QTimer::singleShot(400, this, &MokSignerPage::triggerScan);
                        });

                sbSignWorker->start(program, arguments);
    });

    QPushButton *btnExecuteUnsign = this->findChild<QPushButton*>(QStringLiteral("btnExecuteUnsign"));
    if (btnExecuteUnsign) {
        btnExecuteUnsign->disconnect();
        connect(btnExecuteUnsign, &QPushButton::clicked, this, [this]() {
            QString targetBinary = this->property("selectedKernelPath").toString();

            if (targetBinary.isEmpty()) {
                QMessageBox::warning(this, QStringLiteral("Selection Missing"), QStringLiteral("Please select a target kernel binary from the verified list to unsign first."));
                return;
            }

            QPushButton *btnExecuteUnsignLocal = this->findChild<QPushButton*>(QStringLiteral("btnExecuteUnsign"));
            QTextBrowser *logTerminal = this->findChild<QTextBrowser*>(QStringLiteral("signerLogTerminal"));

            if (logTerminal) {
                logTerminal->clear();
                logTerminal->append(QStringLiteral("⚠️ [SYSTEM INIT]: Launching kernel signature removal pipeline..."));
                logTerminal->append(QString("📂 [TARGET BINARY]: %1").arg(targetBinary));
            }

            if (btnExecuteSignature) btnExecuteSignature->setEnabled(false);
            if (btnExecuteUnsignLocal) {
                btnExecuteUnsignLocal->setEnabled(false);
                btnExecuteUnsignLocal->setText(QStringLiteral("⏳ Stripping Signature..."));
            }
            qApp->processEvents();

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

                    connect(sbUnsignWorker, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
                            [this, sbUnsignWorker, btnExecuteUnsignLocal, logTerminal](int exitCode, QProcess::ExitStatus status) {

                                if (btnExecuteSignature) btnExecuteSignature->setEnabled(true);
                                if (btnExecuteUnsignLocal) {
                                    btnExecuteUnsignLocal->setEnabled(true);
                                    btnExecuteUnsignLocal->setText(QStringLiteral("Strip / Unsign Selected Binary"));
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
                                this->setProperty("selectedKernelPath", QString());
                                QTimer::singleShot(400, this, &MokSignerPage::triggerScan);
                            });

                    sbUnsignWorker->start(program, arguments);
        });
    }
}
