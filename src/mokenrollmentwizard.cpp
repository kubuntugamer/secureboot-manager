#include "mokenrollmentwizard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>

MokEnrollmentWizard::MokEnrollmentWizard(QWidget *parent)
: QDialog(parent)
{
    setWindowTitle("🚨 SECURE BOOT FIRMWARE REGISTRATION WIZARD");
    setModal(true);
    setFixedWidth(520);
    setStyleSheet("background-color: #1e1e24; color: #ffffff; font-family: monospace;");

    buildInterfaceElements();
}

void MokEnrollmentWizard::buildInterfaceElements()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    mainLayout->setSpacing(14);

    QLabel *headerLabel = new QLabel("🔒 MACHINE OWNER KEY (MOK) SYSTEM INTERLOCK", this);
    headerLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #e74c3c;");
    mainLayout->addWidget(headerLabel);

    QLabel *bodyLabel = new QLabel(
        "You are registering a Machine Owner Key (MOK) with your UEFI firmware database.<br><br>"
        "<b>CRITICAL USER REQUIREMENT:</b> On the absolute next reboot, your system will halt and display a dark blue screen titled "
        "<b>Shim UEFI Key Management</b>. You must physically select <b>'Enroll MOK'</b> and type the validation password you define below.",
        this
    );
    bodyLabel->setWordWrap(true);
    bodyLabel->setStyleSheet("font-size: 11px; color: #bdc3c7; line-height: 1.4;");
    mainLayout->addWidget(bodyLabel);

    mainLayout->addWidget(new QLabel("<hr style='border: 0; border-top: 1px solid #3f4142;'>", this));

    // 🔑 STEP 2.1: CREATE THE PRIMARY PASSWORD DEFINE FIELD
    QLabel *passPrompt = new QLabel("1. CHOOSE YOUR TEMPORARY REBOOT PASSWORD:", this);
    passPrompt->setStyleSheet("font-size: 11px; font-weight: bold; color: #ff9f43;");
    mainLayout->addWidget(passPrompt);

    editUserPassword = new QLineEdit(this);
    editUserPassword->setEchoMode(QLineEdit::Password);
    editUserPassword->setPlaceholderText("Create your temporary boot password here...");
    editUserPassword->setStyleSheet("background-color: #15191c; color: #ffffff; border: 1px solid #3f4142; border-radius: 4px; padding: 6px; font-size: 11px;");
    mainLayout->addWidget(editUserPassword);

    // ⏹️ Safety interlock checkboxes
    chkAcknowledgeBlueScreen = new QCheckBox("I understand I must physically register this via a BLUE PRE-BOOT SCREEN on reboot.", this);
    chkAcknowledgeBlueScreen->setStyleSheet("color: #ffaa00; font-size: 11px; font-weight: bold;");
    mainLayout->addWidget(chkAcknowledgeBlueScreen);

    chkAcknowledgeDataLockout = new QCheckBox("I accept that skipping the blue menu or forgetting this pass will lockout custom kernels.", this);
    chkAcknowledgeDataLockout->setStyleSheet("color: #ffaa00; font-size: 11px; font-weight: bold;");
    mainLayout->addWidget(chkAcknowledgeDataLockout);

    // ✍️ STEP 2.2: CREATE THE CONFIRMATION FIELD FOR MUSCLE MEMORY
    QLabel *confirmPrompt = new QLabel("2. RE-TYPE PASSWORD TO CONFIRM MUSCLE MEMORY:", this);
    confirmPrompt->setStyleSheet("font-size: 11px; font-weight: bold; color: #bdc3c7;");
    mainLayout->addWidget(confirmPrompt);

    editPasswordConfirmation = new QLineEdit(this);
    editPasswordConfirmation->setEchoMode(QLineEdit::Password);
    editPasswordConfirmation->setPlaceholderText("Retype the exact same password phrase here...");
    editPasswordConfirmation->setStyleSheet("background-color: #15191c; color: #ffffff; border: 1px solid #3f4142; border-radius: 4px; padding: 6px; font-size: 11px;");
    mainLayout->addWidget(editPasswordConfirmation);

    // Control button layout
    QHBoxLayout *actionLayout = new QHBoxLayout();
    btnAbortSafely = new QPushButton("Abort Action Safely", this);
    btnAbortSafely->setStyleSheet("background-color: #34495e; color: #ffffff; padding: 8px; border-radius: 4px; font-size: 11px; font-weight: bold;");

    btnProceedToSystemImport = new QPushButton("Force Registration Request", this);
    btnProceedToSystemImport->setEnabled(false); // HARD LOCKED BY DEFAULT
    btnProceedToSystemImport->setStyleSheet("background-color: #2c3e50; color: #7f8c8d; padding: 8px; border-radius: 4px; font-size: 11px; font-weight: bold;");

    actionLayout->addWidget(btnAbortSafely);
    actionLayout->addWidget(btnProceedToSystemImport);
    mainLayout->addLayout(actionLayout);

    // Hook up dynamic input listeners
    connect(editUserPassword, &QLineEdit::textChanged, this, &MokEnrollmentWizard::evaluateInterlockState);
    connect(chkAcknowledgeBlueScreen, &QCheckBox::toggled, this, &MokEnrollmentWizard::evaluateInterlockState);
    connect(chkAcknowledgeDataLockout, &QCheckBox::toggled, this, &MokEnrollmentWizard::evaluateInterlockState);
    connect(editPasswordConfirmation, &QLineEdit::textChanged, this, &MokEnrollmentWizard::evaluateInterlockState);

    connect(btnAbortSafely, &QPushButton::clicked, this, &QDialog::reject);
    connect(btnProceedToSystemImport, &QPushButton::clicked, this, &QDialog::accept);
}

void MokEnrollmentWizard::evaluateInterlockState()
{
    QString firstPass = editUserPassword->text().trimmed();
    QString confirmedPass = editPasswordConfirmation->text().trimmed();

    bool passNotEmpty = !firstPass.isEmpty();
    bool checksPassed = chkAcknowledgeBlueScreen->isChecked() && chkAcknowledgeDataLockout->isChecked();
    bool stringMatched = (firstPass == confirmedPass);

    if (passNotEmpty && checksPassed && stringMatched) {
        btnProceedToSystemImport->setEnabled(true);
        btnProceedToSystemImport->setStyleSheet("background-color: #e74c3c; color: #ffffff; padding: 8px; border-radius: 4px; font-size: 11px; font-weight: bold;");
    } else {
        btnProceedToSystemImport->setEnabled(false);
        btnProceedToSystemImport->setStyleSheet("background-color: #2c3e50; color: #7f8c8d; padding: 8px; border-radius: 4px; font-size: 11px; font-weight: bold;");
    }
}

QString MokEnrollmentWizard::enforceSystemEnrollment(QWidget *parent)
{
    MokEnrollmentWizard wizard(parent);
    if (wizard.exec() == QDialog::Accepted) {
        // 💎 FIXED: Extract string out directly from the scope pointer variable within the active lifecycle frame
        if (wizard.editUserPassword) {
            return wizard.editUserPassword->text().trimmed();
        }
    }
    return ""; // Return clean empty string if aborted
}
