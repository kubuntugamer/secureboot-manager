#include "mokremovalwizard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QTextBrowser>
#include <QStandardPaths>
#include <QMessageBox>

MokRemovalWizard::MokRemovalWizard(const QString &keyName, const QString &keySerial, QWidget *parent)
: QDialog(parent), targetKeyName(keyName), targetKeySerial(keySerial)
{
    this->setWindowTitle("⚠️ AUTOMATED MOK DELETION & REMEDIATION WIZARD");
    this->setFixedSize(580, 460);

    // Sovereignty Configuration: Ensure background window canvas remains interactive
    this->setModal(false);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);

    wizardStack = new QStackedWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(wizardStack);

    // Verify system dependencies exist prior to analyzing local disk storage
    isEnvironmentValid = verifySystemToolchain();

    // =========================================================================
    // 🛠️ MULTI-STAGE BLUEPRINT SEQUENCE (FIXED DYNAMIC RE-INSERTION SHIFTS)
    // ASSOCIATED CODE FILES: include/mokremovalwizard.h, src/mokremovalwizard_init.cpp
    // ROADMAP STEP: Mount pages exactly once via stable addWidget layout structures
    // =========================================================================
    buildSelectionPage();          // Stack Index 0
    buildRemediationPage();        // Stack Index 1
    buildPasswordAssignmentPage(); // Stack Index 2

    wizardStack->setCurrentIndex(0); // Force the wizard to explicitly start on the selection list
}

void MokRemovalWizard::buildRemediationPage()
{
    QWidget *page = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(12);

    QLabel *lblTitle = new QLabel("🔄 STEP 2: AUTOMATED KERNEL SIGNATURE PURGE", page);
    lblTitle->setStyleSheet("color: #e67e22; font-weight: bold; font-size: 13px;");
    layout->addWidget(lblTitle);

    // 🔍 Visual Anchor: Set object name so we can update descriptor text strings dynamically
    QLabel *lblDesc = new QLabel(page);
    lblDesc->setObjectName("remediationDescriptorLabel");
    lblDesc->setWordWrap(true);
    layout->addWidget(lblDesc);

    logTerminal = new QTextBrowser(page);
    logTerminal->setReadOnly(true);
    logTerminal->setFont(QFont("monospace", 10));
    logTerminal->setStyleSheet("background-color: #1e1e24; color: #ffffff; border: 1px solid #3f4142;");
    layout->addWidget(logTerminal);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnBack = new QPushButton("⬅️ Back to List", page);
    btnBack->setMinimumHeight(38);
    connect(btnBack, &QPushButton::clicked, this, [this]() { wizardStack->setCurrentIndex(0); });

    btnStartRemediation = new QPushButton(this);
    btnStartRemediation->setObjectName("remediationActionButton");
    btnStartRemediation->setMinimumHeight(38);
    btnStartRemediation->setFont(QFont("", -1, QFont::Bold));

    btnLayout->addWidget(btnBack);
    btnLayout->addWidget(btnStartRemediation);
    layout->addLayout(btnLayout);

    wizardStack->addWidget(page); // 🔍 FIXED: Stable addWidget structure to lock down index positions permanently
}

void MokRemovalWizard::buildPasswordAssignmentPage()
{
    QWidget *page = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(12);

    QLabel *lblTitle = new QLabel("🔒 STEP 3: FIRMWARE DELETION REBOOT CREDENTIALS", page);
    lblTitle->setStyleSheet("color: #ff9f43; font-weight: bold; font-size: 12px;");
    layout->addWidget(lblTitle);

    QLabel *lblDesc = new QLabel(
        "Kernel file blocks are now fully clear. To finalize the removal of the certificate "
        "from your motherboard NVRAM database lanes, choose a temporary password configuration. "
        "You must explicitly type this passphrase on the blue MOKManager screen during your next restart loop.", page);
    lblDesc->setWordWrap(true);
    layout->addWidget(lblDesc);

    editResetPassword = new QLineEdit(page);
    editResetPassword->setEchoMode(QLineEdit::Password);
    editResetPassword->setPlaceholderText("Create temporary boot password...");
    editResetPassword->setMinimumHeight(32);

    editResetConfirm = new QLineEdit(page);
    editResetConfirm->setEchoMode(QLineEdit::Password);
    editResetConfirm->setPlaceholderText("Confirm temporary boot password...");
    editResetConfirm->setMinimumHeight(32);

    layout->addWidget(new QLabel("Assign Temporary Deletion Password:", page));
    layout->addWidget(editResetPassword);
    layout->addWidget(new QLabel("Re-type password configuration:", page));
    layout->addWidget(editResetConfirm);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnBackToScan = new QPushButton("⬅️ Back to Scan", page);
    btnBackToScan->setMinimumHeight(38);

    // =========================================================================
    // 🔄 STACK NAVIGATION GAP RESOLVED
    // ASSOCIATED CODE FILES: src/mokremovalwizard_init.cpp
    // ROADMAP STEP: Maps backwards safely to absolute static Index 1
    // =========================================================================
    connect(btnBackToScan, &QPushButton::clicked, this, [this]() { wizardStack->setCurrentIndex(1); });

    btnExecutePurge = new QPushButton("🗑️ Commit Key Removal Request", page);
    btnExecutePurge->setMinimumHeight(38);
    btnExecutePurge->setStyleSheet("background-color: #c0392b; color: #ffffff; font-weight: bold;");

    connect(btnExecutePurge, &QPushButton::clicked, this, [this]() {
        QString pass = editResetPassword->text();
        QString confirm = editResetConfirm->text();

        editResetPassword->setStyleSheet("");
        editResetConfirm->setStyleSheet("");

        if (pass.isEmpty() || confirm.isEmpty()) {
            editResetPassword->setStyleSheet("border: 1px solid #e74c3c;");
            editResetConfirm->setStyleSheet("border: 1px solid #e74c3c;");
            QMessageBox::warning(this, "Input Required", "<b>Please enter a temporary password.</b>");
            return;
        }

        if (pass != confirm) {
            editResetConfirm->setStyleSheet("border: 1px solid #e74c3c;");
            QMessageBox::critical(this, "Mismatch Detected", "<b>The passwords do not match.</b><br>Please re-type your input.");
            return;
        }

        this->executeMokNVRAMPurge();
    });

    btnLayout->addWidget(btnBackToScan);
    btnLayout->addWidget(btnExecutePurge);
    layout->addLayout(btnLayout);

    wizardStack->addWidget(page);
}

MokRemovalWizard* MokRemovalWizard::executeKeyRevocation(const QString &keyName, const QString &keySerial, QWidget *parent)
{
    MokRemovalWizard *wizard = new MokRemovalWizard(keyName, keySerial, parent);
    wizard->show();
    wizard->raise();
    wizard->activateWindow();
    return wizard;
}
