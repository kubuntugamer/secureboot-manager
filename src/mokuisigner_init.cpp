// ============================================================================
// FRAGMENT 1 OF 2: LAYOUT RESET & DYNAMIC KERNEL MATRIX COLUMNS
// ============================================================================

#include "mokuisigner.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QTextBrowser>

void MokUiSigner::setupSigningPage(QWidget *pageContainer, QLineEdit *&targetPathEdit, QLineEdit *&keyPathEdit,
                                   QPushButton *&browseBinaryBtn, QPushButton *&executeSignBtn,
                                   const QStringList &unsignedLabels, const QStringList &unsignedPaths,
                                   const QStringList &signedLabels, const QStringList &signedPaths)
{
    if (!pageContainer) return;

    // 🧹 Clean layout elements cleanly to stop widget stacking/drift anomalies
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

    QLabel *titleLabel = new QLabel(QStringLiteral("Wizard-Driven Kernel Secure Boot Signer"), pageContainer);
    titleLabel->setStyleSheet(QStringLiteral("font-size: 16px; font-weight: bold; color: #ffffff;"));
    mainLayout->addWidget(titleLabel);

    QHBoxLayout *columnsLayout = new QHBoxLayout();
    columnsLayout->setSpacing(20);

    // Left Column: Staged for Signature Injection
    QVBoxLayout *leftCol = new QVBoxLayout();
    leftCol->setSpacing(10);
    QLabel *leftHeader = new QLabel(QStringLiteral("⚠️ Staged for Signature Injection:"), pageContainer);
    leftHeader->setStyleSheet(QStringLiteral("font-size: 11px; font-weight: bold; color: #ff9f43;"));
    leftCol->addWidget(leftHeader);

    if (unsignedLabels.isEmpty()) {
        QLabel *emptyLabel = new QLabel(QStringLiteral("🔒 All kernels verified.\nNo actions required."), pageContainer);
        emptyLabel->setStyleSheet(QStringLiteral("font-size: 11px; color: #1dd1a1; font-family: monospace; text-align: center; padding: 15px; border: 1px dashed #1e3d30; border-radius: 6px;"));
        leftCol->addWidget(emptyLabel);
    } else {
        for (int i = 0; i < unsignedLabels.size(); ++i) {
            QPushButton *btnCard = new QPushButton(QStringLiteral("🐧 %1\nStaged for MOK Signing").arg(unsignedLabels.at(i)), pageContainer);
            btnCard->setStyleSheet(QStringLiteral("background-color: #1e272e; color: #ffffff; border: 1px solid #3f4142; border-radius: 6px; padding: 12px; font-family: monospace; font-size: 11px; text-align: left;"));

            QString targetPath = (i < unsignedPaths.size()) ? unsignedPaths.at(i) : QString();
            QObject::connect(btnCard, &QPushButton::clicked, pageContainer, [pageContainer, targetPath]() {
                pageContainer->setProperty("selectedKernelPath", targetPath);
                if (auto log = pageContainer->findChild<QTextBrowser*>(QStringLiteral("signerLogTerminal"))) {
                    log->append(QStringLiteral("📌 [STAGED TARGET SELECTED]: ") + targetPath);
                }
            });

            leftCol->addWidget(btnCard);
        }
    }
    leftCol->addStretch();

    // Right Column: Currently Verified & Protected
    QVBoxLayout *rightCol = new QVBoxLayout();
    rightCol->setSpacing(10);
    QLabel *rightHeader = new QLabel(QStringLiteral("🔒 Currently Verified & Protected (Click to Select for Unsigning):"), pageContainer);
    rightHeader->setStyleSheet(QStringLiteral("font-size: 11px; font-weight: bold; color: #1dd1a1;"));
    rightCol->addWidget(rightHeader);

    QListWidget *verifiedListWidget = new QListWidget(pageContainer);
    verifiedListWidget->setObjectName(QStringLiteral("verifiedListWidget"));
    verifiedListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    verifiedListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    verifiedListWidget->setMinimumHeight(140);
    verifiedListWidget->setStyleSheet(
        QStringLiteral(
            "QListWidget {"
            "  background-color: #15191c; border: 1px solid #1e3d30; border-radius: 6px; padding: 5px; color: #1dd1a1; font-family: monospace; font-size: 11px; font-weight: bold;"
            "}"
            "QListWidget::item {"
            "  padding: 8px; border-bottom: 1px solid #1a2226;"
            "}"
            "QListWidget::item:selected {"
            "  background-color: #1e3d30; color: #ffffff; border: 1px solid #1dd1a1; border-radius: 4px;"
            "}"
        )
    );

    if (signedLabels.isEmpty()) {
        QListWidgetItem *item = new QListWidgetItem(QStringLiteral("No signed kernels detected."), verifiedListWidget);
        item->setFlags(Qt::NoItemFlags);
        item->setForeground(QBrush(QColor(QStringLiteral("#7f8c8d"))));
    } else {
        for (int i = 0; i < signedLabels.size(); ++i) {
            QListWidgetItem *item = new QListWidgetItem(QStringLiteral("🔒 %1 [Verified & Protected]").arg(signedLabels.at(i)), verifiedListWidget);
            QString pairedSignedPath = (i < signedPaths.size()) ? signedPaths.at(i) : QString();
            item->setData(Qt::UserRole, pairedSignedPath);
        }

        QObject::connect(verifiedListWidget, &QListWidget::itemClicked, pageContainer, [pageContainer](QListWidgetItem *item) {
            QString resolvedBootPath = item->data(Qt::UserRole).toString();
            pageContainer->setProperty("selectedKernelPath", resolvedBootPath);

            if (auto log = pageContainer->findChild<QTextBrowser*>("signerLogTerminal")) {
                log->append(QStringLiteral("📌 [PROTECTED TARGET SELECTED]: ") + resolvedBootPath);
            }
        });
    }

    rightCol->addWidget(verifiedListWidget);
    rightCol->addStretch();

    columnsLayout->addLayout(leftCol, 1);
    columnsLayout->addLayout(rightCol, 1);
    mainLayout->addLayout(columnsLayout);
    // ============================================================================
    // FRAGMENT 2 OF 2: CLEAN BUTTON HOOKS & DIAGNOSTICS CONSOLE TERMINAL
    // ============================================================================

    mainLayout->addSpacing(5);
    mainLayout->addWidget(new QLabel(QStringLiteral("<hr style='border: 0; border-top: 1px solid #3f4142;'>"), pageContainer));

    // 🛠️ Nullify legacy pointers cleanly to avoid layout overlap loops or duplicate input fields
    targetPathEdit = nullptr;
    keyPathEdit = nullptr;
    browseBinaryBtn = nullptr;

    executeSignBtn = new QPushButton(QStringLiteral("Inject Secure Boot Signature"), pageContainer);
    executeSignBtn->setStyleSheet(QStringLiteral("background-color: #2a3b4d; color: #3daee9; border: 1px solid #3daee9; font-weight: bold; padding: 12px; border-radius: 4px; font-size: 11px;"));
    mainLayout->addWidget(executeSignBtn);

    mainLayout->addSpacing(8);
    mainLayout->addWidget(new QLabel(QStringLiteral("Real-Time Cryptographic Signature Diagnostics Console:"), pageContainer));

    QTextBrowser *signerLogTerminal = new QTextBrowser(pageContainer);
    signerLogTerminal->setObjectName(QStringLiteral("signerLogTerminal"));
    signerLogTerminal->setReadOnly(true);
    signerLogTerminal->setMinimumHeight(120);
    signerLogTerminal->setStyleSheet(QStringLiteral("background-color: #15191c; color: #3daee9; border: 1px solid #2c3e50; border-radius: 4px; padding: 8px; font-family: monospace; font-size: 10pt;"));
    mainLayout->addWidget(signerLogTerminal);
}
