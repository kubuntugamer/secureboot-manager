#include "mokremovalwizard.h"
#include "mokprovider.h" // 🔒 Core Systems Logic Alignment: Pulls verified active NVRAM signatures
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QListWidget>
#include <QMessageBox>
#include <QTextBrowser>

// =========================================================================
// 📋 BLUEPRINT STAGE 1: SELECTION LEDGER OVERLAY PAGE
// ASSOCIATED CODE FILES: include/mokremovalwizard.h, src/mokremovalwizard_ledger.cpp
// ROADMAP STEP: Filter out distribution vendor keys and update scan text browsers dynamically
// =========================================================================
void MokRemovalWizard::buildSelectionPage()
{
    QWidget *page = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(12);

    QLabel *lblTitle = new QLabel("📋 STEP 1: SELECT CUSTOM KEY FOR REMEDIATION", page);
    lblTitle->setStyleSheet("color: #3498db; font-weight: bold; font-size: 13px;");
    layout->addWidget(lblTitle);

    QLabel *lblDesc = new QLabel("The ledger below represents the active custom Machine Owner Keys currently discovered "
    "in firmware storage rows. Select a signature target to continue:", page);
    lblDesc->setWordWrap(true);
    layout->addWidget(lblDesc);

    keyListWidget = new QListWidget(page);
    keyListWidget->setStyleSheet("background-color: #1e1e24; color: #ffffff; border: 1px solid #3f4142; padding: 4px;");
    layout->addWidget(keyListWidget);

    // 📡 Live NVRAM Data Acquisition Loop using the static engine provider
    QVector<MokKeyEntry> systemKeys = MokProvider::getLiveKeys();
    for (const auto &key : systemKeys) {

        // 🛡️ SOVEREIGNTY FILTER: Do not list official distribution vendor or system validation keys
        if (key.commonName.contains("Canonical Ltd.", Qt::CaseInsensitive) ||
            key.commonName.contains("Debian", Qt::CaseInsensitive)         ||
            key.commonName.contains("Fedora", Qt::CaseInsensitive)         ||
            key.commonName.contains("Red Hat", Qt::CaseInsensitive)) {
            continue; // Safely skip this entry so the user cannot accidentally target it
            }

            QString itemDisplay = QString("🔑 CN: %1\n   Serial: %2").arg(key.commonName, key.serialNumber);
        QListWidgetItem *item = new QListWidgetItem(itemDisplay, keyListWidget);

        // Cache parameters securely in standard user memory roles for rapid lookup extraction
        item->setData(Qt::UserRole, key.commonName);
        item->setData(Qt::UserRole + 1, key.serialNumber);
    }

    // Default item safety feedback if firmware database query returns completely empty lines
    if (keyListWidget->count() == 0) {
        QListWidgetItem *emptyNotice = new QListWidgetItem("❌ [No custom MOK keys found in firmware ledger]", keyListWidget);
        emptyNotice->setFlags(Qt::NoItemFlags);
    }

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnCancel = new QPushButton("Cancel Safely", page);
    btnCancel->setMinimumHeight(38);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    QPushButton *btnNext = new QPushButton("Analyze Key Dependencies ➡️", page);
    btnNext->setMinimumHeight(38);
    btnNext->setStyleSheet("background-color: #3498db; color: #ffffff; font-weight: bold;");

    // User Autonomy Interlock: Keep button disabled until an asset row choice is explicitly made
    btnNext->setEnabled(false);
    connect(keyListWidget, &QListWidget::itemSelectionChanged, this, [btnNext, this]() {
        btnNext->setEnabled(keyListWidget->currentItem() != nullptr);
    });

    connect(btnNext, &QPushButton::clicked, this, [this]() {
        QListWidgetItem *current = keyListWidget->currentItem();
        if (!current) return;

        // Dynamically override initialization strings with the selected target parameters
        targetKeyName = current->data(Qt::UserRole).toString();
        targetKeySerial = current->data(Qt::UserRole + 1).toString();

        // Re-execute file picker scanning checks based on the fresh selection input
        if (isEnvironmentValid) {
            scanForSignedBinaries();
        }

        // =========================================================================
        // 🔒 STABLE STACK TRANSLATION: UPDATE VIEW FIELDS CONCURRENTLY
        // ASSOCIATED CODE FILES: src/mokremovalwizard_ledger.cpp
        // ROADMAP STEP: Query widgets by name to refresh descriptions safely
        // =========================================================================
        QLabel *lblRemediationDesc = wizardStack->widget(1)->findChild<QLabel*>("remediationDescriptorLabel");
        QPushButton *btnRemediationAction = wizardStack->widget(1)->findChild<QPushButton*>("remediationActionButton");

        if (logTerminal) {
            logTerminal->clear();
        }

        if (!isEnvironmentValid) {
            if (lblRemediationDesc) {
                lblRemediationDesc->setText("<b style='color: #e74c3c;'>Dependency Validation Failure</b><br><br>"
                "The wizard cannot initialize because necessary cryptographic binaries are missing or restricted.");
            }
            if (logTerminal) {
                logTerminal->append("<span style='color: #e74c3c;'>[FATAL] Missing required target tools:</span>");
            }
            if (btnRemediationAction) {
                btnRemediationAction->setText("Execution Blocked");
                btnRemediationAction->setEnabled(false);
                btnRemediationAction->setStyleSheet("background-color: #3f4142; color: #888888;");
            }
        }
        else if (affectedKernels.isEmpty()) {
            if (lblRemediationDesc) {
                lblRemediationDesc->setText(QString("No active kernel dependencies detected for MOK: <b>[%1]</b>.<br><br>"
                "It is completely safe to bypass the unsigning loop and proceed straight to the firmware removal phase.").arg(targetKeyName));
            }
            if (logTerminal) {
                logTerminal->append("<span style='color: #2ecc71;'>[READY] Dependencies clear. Ready to proceed.</span>");
            }
            if (btnRemediationAction) {
                btnRemediationAction->setText("Advance to MOK Deletion ➡️");
                btnRemediationAction->setStyleSheet("background-color: #27ae60; color: #ffffff;");
                btnRemediationAction->setEnabled(true);
                // FIX: Stale target disconnect macro chains completely removed.
                // Runtime logic behavior is safely handled downstream inside onRemediationActionRouteTriggered().
            }
        } else {
            if (lblRemediationDesc) {
                lblRemediationDesc->setText(QString("The following custom kernels are actively signed by <b>[%1]</b>. "
                "To prevent system boot failure, the wizard will automatically strip these signatures "
                "using administrative privileges prior to removing the key:").arg(targetKeyName));
            }
            if (logTerminal) {
                logTerminal->append("<b style='color: #ff9f43;'>Staged targets for signature extraction:</b>");
                for (const QString &kernel : affectedKernels) {
                    logTerminal->append(QString("• %1").arg(kernel));
                }
            }
            if (btnRemediationAction) {
                btnRemediationAction->setText("🛠️ Purge Signatures & Proceed");
                btnRemediationAction->setStyleSheet("background-color: #2980b9; color: #ffffff;");
                btnRemediationAction->setEnabled(true);
                // FIX: Dynamic slot overrides removed to preserve connection metrics on repeated runs
            }
        }

        // Shift view index securely to the file scanning feedback pane
        wizardStack->setCurrentIndex(1);
    });

    btnLayout->addWidget(btnCancel);
    btnLayout->addWidget(btnNext);
    layout->addLayout(btnLayout);

    wizardStack->addWidget(page);
}

// FIX: Added unified routing tracking slot to process actions based on stable state checks
void MokRemovalWizard::onRemediationActionRouteTriggered()
{
    if (!isEnvironmentValid) return;

    if (affectedKernels.isEmpty()) {
        // Safe bypass transition straight to Step 3 (Index 2)
        wizardStack->setCurrentIndex(2);
    } else {
        // Launch signature extraction pipeline
        handleRemediationPipeline();
    }
}
