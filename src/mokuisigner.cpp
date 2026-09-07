#include "mokuisigner.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QDir>
#include <QTextBrowser>
#include <QProcess>
#include <QFile>
#include <QDebug>
#include <QBrush>
#include <QFileDialog> // Included for native file picker dialog interface integration

void MokUiSigner::setupSigningPage(QWidget *pageContainer, QLineEdit *&targetPathEdit, QLineEdit *&keyPathEdit,
                                   QPushButton *&browseBinaryBtn, QPushButton *&executeSignBtn,
                                   const QStringList &unsignedLabels, const QStringList &unsignedPaths,
                                   const QStringList &signedLabels, const QStringList &signedPaths)
{
    if (!pageContainer) return;

    // 🧼 Purge old layout elements
    qDeleteAll(pageContainer->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly));
    if (pageContainer->layout()) {
        QLayoutItem *item;
        while ((item = pageContainer->layout()->takeAt(0)) != nullptr) {
            delete item;
        }
        delete pageContainer->layout();
    }

    QVBoxLayout *mainLayout = new QVBoxLayout(pageContainer);
    mainLayout->setContentsMargins(30, 25, 30, 25);
    mainLayout->setSpacing(14);

    QLabel *titleLabel = new QLabel("Wizard-Driven Kernel Secure Boot Signer", pageContainer);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #ffffff;");
    mainLayout->addWidget(titleLabel);

    QHBoxLayout *columnsLayout = new QHBoxLayout();
    columnsLayout->setSpacing(20);

    // Left Column: Staged for Signature Injection
    QVBoxLayout *leftCol = new QVBoxLayout();
    leftCol->setSpacing(10);
    QLabel *leftHeader = new QLabel("⚠️ Staged for Signature Injection:", pageContainer);
    leftHeader->setStyleSheet("font-size: 11px; font-weight: bold; color: #ff9f43;");
    leftCol->addWidget(leftHeader);

    if (unsignedLabels.isEmpty()) {
        QLabel *emptyLabel = new QLabel("🔒 All kernels verified.\nNo actions required.", pageContainer);
        emptyLabel->setStyleSheet("font-size: 11px; color: #1dd1a1; font-family: monospace; text-align: center; padding: 15px; border: 1px dashed #1e3d30; border-radius: 6px;");
        leftCol->addWidget(emptyLabel);
    } else {
        for (int i = 0; i < unsignedLabels.size(); ++i) {
            QPushButton *btnCard = new QPushButton(QString("🐧 %1\nStaged for MOK Signing").arg(unsignedLabels.at(i)), pageContainer);
            btnCard->setStyleSheet("background-color: #1e272e; color: #ffffff; border: 1px solid #3f4142; border-radius: 6px; padding: 12px; font-family: monospace; font-size: 11px; text-align: left;");

            QString targetPath = (i < unsignedPaths.size()) ? unsignedPaths.at(i) : QString();
            QObject::connect(btnCard, &QPushButton::clicked, pageContainer, [pageContainer, targetPath]() {
                pageContainer->setProperty("selectedKernelPath", targetPath);
                if (auto log = pageContainer->findChild<QTextBrowser*>("signerLogTerminal")) {
                    log->append(QString("📌 [STAGED TARGET SELECTED]: %1").arg(targetPath));
                }
            });

            leftCol->addWidget(btnCard);
        }
    }
    leftCol->addStretch();

    // Right Column: Currently Verified & Protected
    QVBoxLayout *rightCol = new QVBoxLayout();
    rightCol->setSpacing(10);
    QLabel *rightHeader = new QLabel("🔒 Currently Verified & Protected (Click to Select for Unsigning):", pageContainer);
    rightHeader->setStyleSheet("font-size: 11px; font-weight: bold; color: #1dd1a1;");
    rightCol->addWidget(rightHeader);

    QListWidget *verifiedListWidget = new QListWidget(pageContainer);
    verifiedListWidget->setObjectName("verifiedListWidget");
    verifiedListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    verifiedListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    verifiedListWidget->setMinimumHeight(140);
    verifiedListWidget->setStyleSheet(
        "QListWidget {"
        "  background-color: #15191c; border: 1px solid #1e3d30; border-radius: 6px; padding: 5px; color: #1dd1a1; font-family: monospace; font-size: 11px; font-weight: bold;"
        "}"
        "QListWidget::item {"
        "  padding: 8px; border-bottom: 1px solid #1a2226;"
        "}"
        "QListWidget::item:selected {"
        "  background-color: #1e3d30; color: #ffffff; border: 1px solid #1dd1a1; border-radius: 4px;"
        "}"
    );

    if (signedLabels.isEmpty()) {
        QListWidgetItem *item = new QListWidgetItem("No signed kernels detected.", verifiedListWidget);
        item->setFlags(Qt::NoItemFlags);
        item->setForeground(QBrush(QColor("#7f8c8d")));
    } else {
        for (int i = 0; i < signedLabels.size(); ++i) {
            QListWidgetItem *item = new QListWidgetItem(QString("🔒 %1 [Verified & Protected]").arg(signedLabels.at(i)), verifiedListWidget);

            // 💎 FIX: Map the dynamic verified path token from the discovery engine directly into the item data role string vector
            QString pairedSignedPath = (i < signedPaths.size()) ? signedPaths.at(i) : QString();
            item->setData(Qt::UserRole, pairedSignedPath);
        }

        // Native index listener assignment mapping
        QObject::connect(verifiedListWidget, &QListWidget::itemClicked, pageContainer, [pageContainer](QListWidgetItem *item) {
            QString resolvedBootPath = item->data(Qt::UserRole).toString();

            pageContainer->setProperty("selectedKernelPath", resolvedBootPath);

            if (auto log = pageContainer->findChild<QTextBrowser*>("signerLogTerminal")) {
                log->append(QString("📌 [PROTECTED TARGET SELECTED]: %1").arg(resolvedBootPath));
            }
        });
    }

    rightCol->addWidget(verifiedListWidget);
    rightCol->addStretch();

    columnsLayout->addLayout(leftCol, 1);
    columnsLayout->addLayout(rightCol, 1);
    mainLayout->addLayout(columnsLayout);

    mainLayout->addSpacing(5);
    mainLayout->addWidget(new QLabel("<hr style='border: 0; border-top: 1px solid #3f4142;'>", pageContainer));

    targetPathEdit = nullptr;
    browseBinaryBtn = nullptr;

    keyPathEdit = new QLineEdit(pageContainer);
    keyPathEdit->setText("/var/lib/shim-signed/mok/MOK.priv");
    keyPathEdit->setStyleSheet("background-color: #1e1e24; color: #ffffff; border: 1px solid #3f4142; border-radius: 4px; padding: 6px; font-size: 11px;");
    mainLayout->addWidget(new QLabel("Staged Authorization Key Asset Path:", pageContainer));

    // 🔑 HORIZONTAL BUTTON MIX: Bundles the input text field and the browse action into a single bar row layout
    QHBoxLayout *keyRowLayout = new QHBoxLayout();
    keyRowLayout->setSpacing(8);

    QPushButton *btnBrowseKey = new QPushButton("Browse...", pageContainer);
    btnBrowseKey->setStyleSheet(
        "QPushButton {"
        "  background-color: #2c3e50; color: #ffffff; border: 1px solid #3f4142;"
        "  font-weight: bold; padding: 6px 14px; border-radius: 4px; font-size: 11px;"
        "}"
        "QPushButton:hover { background-color: #34495e; }"
    );

    // Arrange the components side by side
    keyRowLayout->addWidget(keyPathEdit, 1); // Gives the text field maximum stretching expansion priority
    keyRowLayout->addWidget(btnBrowseKey);
    mainLayout->addLayout(keyRowLayout);

    // 📁 NATIVE DESKTOP FILE SYSTEM DIALOG CONNECTED STREAM
    QObject::connect(btnBrowseKey, &QPushButton::clicked, pageContainer, [keyPathEdit, pageContainer]() {
        QString selectedFile = QFileDialog::getOpenFileName(
            pageContainer,
            "Select Secure Boot Authorization Asset Key",
            "/var/lib/shim-signed/mok", // Default directory path cache context mapping
            "MOK Credentials (*.priv *.key *.der *.pem);;All Files (*)"
        );

        if (!selectedFile.isEmpty()) {
            keyPathEdit->setText(selectedFile.trimmed());
        }
    });

    executeSignBtn = new QPushButton("Inject Secure Boot Signature", pageContainer);
    executeSignBtn->setStyleSheet("background-color: #2a3b4d; color: #3daee9; border: 1px solid #3daee9; font-weight: bold; padding: 12px; border-radius: 4px; font-size: 11px;");
    mainLayout->addWidget(executeSignBtn);

    mainLayout->addSpacing(8);
    mainLayout->addWidget(new QLabel("Real-Time Cryptographic Signature Diagnostics Console:", pageContainer));

    QTextBrowser *signerLogTerminal = new QTextBrowser(pageContainer);
    signerLogTerminal->setObjectName("signerLogTerminal");
    signerLogTerminal->setReadOnly(true);
    signerLogTerminal->setMinimumHeight(120);
    signerLogTerminal->setStyleSheet("background-color: #15191c; color: #3daee9; border: 1px solid #2c3e50; border-radius: 4px; padding: 8px; font-family: monospace; font-size: 10pt;");
    mainLayout->addWidget(signerLogTerminal);
}

bool MokUiSigner::unsignKernelBinary(const QString &kernelPath)
{
    if (kernelPath.isEmpty() || !QFile::exists(kernelPath)) {
        qWarning() << "Invalid or non-existent kernel path provided for unsigning:" << kernelPath;
        return false;
    }

    QStringList arguments;
    arguments << "sbattach" << "--remove" << kernelPath;

    QProcess process;
    process.start("pkexec", arguments);

    if (!process.waitForStarted(3000)) {
        qCritical() << "Failed to start pkexec process for signature removal.";
        return false;
    }

    process.waitForFinished(-1);

    if (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0) {
        qDebug() << "Successfully removed signature from kernel binary:" << kernelPath;
        return true;
    } else {
        QString errorOutput = QString::fromUtf8(process.readAllStandardError());
        qCritical() << "Failed to unsign kernel binary. Exit code:" << process.exitCode() << "Error:" << errorOutput.trimmed();
        return false;
    }
}
