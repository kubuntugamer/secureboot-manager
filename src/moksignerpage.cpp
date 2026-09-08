#include "moksignerpage.h"
#include "mokuisigner.h"
#include "abstractdiscoveryengine.h"
#include "debiandiscoveryengine.h"
#include "keyscanner.h"
#include "signingengine.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QTimer>
#include <QApplication>
#include <QMessageBox>
#include <QTextBrowser>
#include <QLabel>
#include <QVariant>
#include <QFile>
#include <QDir>

MokSignerPage::MokSignerPage(QWidget *parent)
: QWidget(parent)
, m_keyComboBox(nullptr)
{
    if (QFile::exists(QStringLiteral("/usr/bin/dpkg-query"))) {
        discoveryEngine = new DebianDiscoveryEngine(this);
    } else {
        return;
    }

    // 🎨 UI INITIALIZATION: Draw the secure authentication greeting screen
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
    btnInitScan->setStyleSheet(QStringLiteral("QPushButton { background-color: #2c3e50; color: #3daee9; border: 1px solid #3daee9; font-weight: bold; padding: 14px; border-radius: 4px; font-size: 12px; } QPushButton:hover { background-color: #3daee9; color: #ffffff; }"));
    initialLayout->addWidget(btnInitScan);
    initialLayout->addStretch();

    // 🔄 CORE REBUILD HANDLER: Rebuilds screen grid once system validation finishes
    connect(discoveryEngine, &AbstractDiscoveryEngine::discoveryFinished, this, [=](QStringList uNames, QStringList uPaths, QStringList sNames) {
        QString existingLogText;
        if (auto oldLog = this->findChild<QTextBrowser*>(QStringLiteral("signerLogTerminal"))) {
            existingLogText = oldLog->toPlainText();
        }

        // Wipe layout welcome assets clean off the widget tree space
        QList<QWidget*> lingeringChildren = this->findChildren<QWidget*>();
        for (QWidget* child : lingeringChildren) { child->setParent(nullptr); child->deleteLater(); }
        if (this->layout()) { delete this->layout(); }

        QStringList resolvedSignedPaths;
        QDir bootDir(QStringLiteral("/boot"));
        for (const QString &signedName : sNames) {
            resolvedSignedPaths.append(bootDir.filePath(QStringLiteral("vmlinuz-") + signedName));
        }

        QLineEdit *dummyLegacyLine = nullptr;
        MokUiSigner::setupSigningPage(this, editSignTargetPath, dummyLegacyLine, btnBrowseBinary, btnExecuteSignature, uNames, uPaths, sNames, resolvedSignedPaths);

        m_keyComboBox = new QComboBox(this);
        m_keyComboBox->setMinimumWidth(350);
        m_keyComboBox->setStyleSheet(QStringLiteral("QComboBox { background-color: #1e272e; color: #ffffff; border: 1px solid #3daee9; padding: 8px; border-radius: 4px; font-family: monospace; } QComboBox:disabled { background-color: #2f3542; color: #747d8c; border: 1px solid #747d8c; }"));

        if (auto mainLayout = qobject_cast<QVBoxLayout*>(this->layout())) {
            QHBoxLayout *selectorLayout = new QHBoxLayout();
            QLabel *selectorLabel = new QLabel(QStringLiteral("🔑 Selected Secure Boot Key Profile:"), this);
            selectorLabel->setStyleSheet(QStringLiteral("color: #ffffff; font-weight: bold; font-size: 11px;"));
            selectorLayout->addWidget(selectorLabel);
            selectorLayout->addWidget(m_keyComboBox, 1);

            int runIdx = mainLayout->indexOf(btnExecuteSignature);
            if (runIdx != -1) mainLayout->insertLayout(runIdx, selectorLayout);

            mainLayout->removeWidget(btnExecuteSignature);
            QHBoxLayout *buttonLayout = new QHBoxLayout();
            buttonLayout->setSpacing(10);

            QPushButton *btnExecuteUnsign = new QPushButton(QStringLiteral("Strip / Unsign Selected Binary"), this);
            btnExecuteUnsign->setObjectName(QStringLiteral("btnExecuteUnsign"));
            btnExecuteUnsign->setStyleSheet(QStringLiteral("QPushButton { background-color: #3d2222; color: #ff6b6b; border: 1px solid #ff6b6b; font-weight: bold; padding: 12px; border-radius: 4px; font-size: 11px; } QPushButton:hover { background-color: #ff6b6b; color: #ffffff; }"));

            buttonLayout->addWidget(btnExecuteSignature, 1);
            buttonLayout->addWidget(btnExecuteUnsign, 1);
            if (runIdx != -1) mainLayout->insertLayout(runIdx, buttonLayout);
        }

        populateKeySelector();
        if (!existingLogText.isEmpty()) {
            if (auto newLog = this->findChild<QTextBrowser*>(QStringLiteral("signerLogTerminal"))) newLog->setPlainText(existingLogText);
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
void MokSignerPage::populateKeySelector()
{
    if (!m_keyComboBox) return;
    m_keyComboBox->clear();

    // ⚡ DELEGATED SCAN: Let KeyScanner handle all directory reading and OpenSSL tasks
    m_keyProfiles = KeyScanner::scanForKeys();

    for (const KeyProfile &profile : m_keyProfiles) {
        m_keyComboBox->addItem(profile.displayName, QVariant::fromValue(profile.privateKeyPath));
    }

    if (m_keyComboBox->count() == 0) {
        m_keyComboBox->addItem(QStringLiteral("No keys found. Place profiles in ~/secureboot-manager-keys/"), QString());
        m_keyComboBox->setEnabled(false);
    } else {
        m_keyComboBox->setEnabled(true);
        for (int i = 0; i < m_keyComboBox->count(); ++i) {
            QString itemText = m_keyComboBox->itemText(i);
            if (itemText.contains(QStringLiteral("Master"), Qt::CaseInsensitive) || itemText.contains(QStringLiteral("Custom"), Qt::CaseInsensitive)) {
                m_keyComboBox->setCurrentIndex(i);
                break;
            }
        }
    }
}

void MokSignerPage::triggerScan()
{
    if (discoveryEngine) discoveryEngine->discoverKernels();
}

void MokSignerPage::setupExecutionHook()
{
    if (!btnExecuteSignature) return;

    // ⚡ DELEGATED RUNNER: Connect visual clicks directly into standalone process classes
    SigningEngine *engine = new SigningEngine(this);
    QTextBrowser *logTerminal = this->findChild<QTextBrowser*>(QStringLiteral("signerLogTerminal"));

    if (logTerminal) {
        connect(engine, &SigningEngine::logMessageReceived, logTerminal, &QTextBrowser::append);
    }

    connect(engine, &SigningEngine::operationFinished, this, [=](bool success, const QString &message) {
        if (btnExecuteSignature) {
            btnExecuteSignature->setEnabled(true);
            btnExecuteSignature->setText(QStringLiteral("Inject Secure Boot Signature"));
        }
        QPushButton *btnUnsign = this->findChild<QPushButton*>(QStringLiteral("btnExecuteUnsign"));
        if (btnUnsign) btnUnsign->setEnabled(true);

        if (success) {
            QMessageBox::information(this, QStringLiteral("Operation Successful"), message);
        } else {
            QMessageBox::critical(this, QStringLiteral("Operation Failed"), message);
        }

        this->setProperty("selectedKernelPath", QString());
        QTimer::singleShot(400, this, &MokSignerPage::triggerScan);
    });

    connect(btnExecuteSignature, &QPushButton::clicked, this, [=]() {
        QString targetBinary = this->property("selectedKernelPath").toString();
        int activeIdx = m_keyComboBox->currentIndex();

        if (targetBinary.isEmpty() || activeIdx == -1) {
            QMessageBox::warning(this, QStringLiteral("Selection Missing"), QStringLiteral("Please select a target kernel and key profile first."));
            return;
        }

        KeyProfile profile = m_keyProfiles.at(activeIdx);
        if (profile.certificatePath.isEmpty() || !QFile::exists(profile.certificatePath)) {
            QMessageBox::critical(this, QStringLiteral("Certificate Missing"), QStringLiteral("Could not resolve the public certificate file paired to this key."));
            return;
        }

        if (logTerminal) logTerminal->clear();
        btnExecuteSignature->setEnabled(false);
        QPushButton *btnUnsign = this->findChild<QPushButton*>(QStringLiteral("btnExecuteUnsign"));
        if (btnUnsign) btnUnsign->setEnabled(false);
        btnExecuteSignature->setText(QStringLiteral("⏳ Processing Atomic Signature Pipeline... Please Wait."));

        engine->injectSignature(targetBinary, profile.privateKeyPath, profile.certificatePath);
    });

    QPushButton *btnExecuteUnsign = this->findChild<QPushButton*>(QStringLiteral("btnExecuteUnsign"));
    if (btnExecuteUnsign) {
        connect(btnExecuteUnsign, &QPushButton::clicked, this, [=]() {
            QString targetBinary = this->property("selectedKernelPath").toString();
            if (targetBinary.isEmpty()) {
                QMessageBox::warning(this, QStringLiteral("Selection Missing"), QStringLiteral("Please select a target kernel binary to unsign first."));
                return;
            }

            if (logTerminal) logTerminal->clear();
            btnExecuteSignature->setEnabled(false);
            btnExecuteUnsign->setEnabled(false);
            btnExecuteUnsign->setText(QStringLiteral("⏳ Stripping Signature..."));

            engine->stripSignature(targetBinary);
        });
    }
}
