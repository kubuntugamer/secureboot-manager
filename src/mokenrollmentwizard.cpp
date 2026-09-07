#include "mokenrollmentwizard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

MokEnrollmentWizard::MokEnrollmentWizard(QWidget *parent)
: QDialog(parent)
{
    buildInterfaceElements();
}

void MokEnrollmentWizard::buildInterfaceElements()
{
    this->setWindowTitle("⚠️ SECURE BOOT FIRMWARE REGISTRATION WIZARD");
    this->setFixedSize(540, 420);
    this->setModal(true);

    QVBoxLayout *masterLayout = new QVBoxLayout(this);
    masterLayout->setContentsMargins(20, 20, 20, 20);
    masterLayout->setSpacing(15);

    QLabel *titleLabel = new QLabel("🔒 MACHINE OWNER KEY (MOK) SYSTEM INTERLOCK", this);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(11);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #e74c3c;");
    masterLayout->addWidget(titleLabel);

    QLabel *descLabel = new QLabel(
        "You are staging a Machine Owner Key (MOK) signature with your UEFI database.\n\n"
        "CRITICAL USER REQUIREMENT: On the absolute next system reboot, your system will halt "
        "and display a dark blue screen titled MOKManager. You must type this password exactly.", this);
    descLabel->setWordWrap(true);
    masterLayout->addWidget(descLabel);

    QVBoxLayout *passBlock = new QVBoxLayout();
    passBlock->setSpacing(6);

    QLabel *lblPass = new QLabel("1. CHOOSE YOUR TEMPORARY REBOOT PASSWORD:", this);
    lblPass->setStyleSheet("color: #ff9f43; font-weight: bold;");
    editUserPassword = new QLineEdit(this);
    editUserPassword->setEchoMode(QLineEdit::Password);
    editUserPassword->setPlaceholderText("Create your temporary boot password here...");
    editUserPassword->setMinimumHeight(32);

    QLabel *lblConfirm = new QLabel("2. RE-TYPE PASSWORD TO CONFIRM MUSCLE MEMORY:", this);
    lblConfirm->setStyleSheet("color: #ff9f43; font-weight: bold;");
    editPasswordConfirmation = new QLineEdit(this);
    editPasswordConfirmation->setEchoMode(QLineEdit::Password);
    editPasswordConfirmation->setPlaceholderText("Retype the exact same password phrase here...");
    editPasswordConfirmation->setMinimumHeight(32);

    passBlock->addWidget(lblPass);
    passBlock->addWidget(editUserPassword);
    passBlock->addWidget(lblConfirm);
    passBlock->addWidget(editPasswordConfirmation);
    masterLayout->addLayout(passBlock);

    warningBanner = new QLabel(
        "⚠️ CRITICAL SYSTEM WARNING ⚠️\n"
        "FORGETTING OR SKIPPING THIS PASSWORD ON REBOOT WILL PREVENT YOUR KEY FROM ENROLLING, "
        "CAUSING CUSTOM LINUX KERNELS TO FAIL SECURE BOOT INTEGRITY LOOPS INSTANTLY.", this);
    warningBanner->setWordWrap(true);
    warningBanner->setAlignment(Qt::AlignCenter);
    warningBanner->setStyleSheet(
        "background-color: #2c1e1e;"
        "border: 2px solid #e74c3c;"
        "border-radius: 4px;"
        "color: #ff6b6b;"
        "font-weight: bold;"
        "padding: 10px;"
        "font-size: 11px;"
    );
    masterLayout->addWidget(warningBanner);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);

    btnAbortSafely = new QPushButton("Abort Action Safely", this);
    btnAbortSafely->setMinimumHeight(38);
    connect(btnAbortSafely, &QPushButton::clicked, this, &QDialog::reject);

    btnProceedToSystemImport = new QPushButton("⚠️ Acknowledge Risks & Request Registration", this);
    btnProceedToSystemImport->setMinimumHeight(38);
    QFont actFont = btnProceedToSystemImport->font();
    actFont.setBold(true);
    btnProceedToSystemImport->setFont(actFont);
    btnProceedToSystemImport->setStyleSheet("background-color: #34495e; color: #ffffff;");
    connect(btnProceedToSystemImport, &QPushButton::clicked, this, &MokEnrollmentWizard::handleRegistrationSequence);

    btnLayout->addWidget(btnAbortSafely);
    btnLayout->addWidget(btnProceedToSystemImport);
    masterLayout->addLayout(btnLayout);
}

void MokEnrollmentWizard::handleRegistrationSequence()
{
    QString pass = editUserPassword->text();
    QString confirm = editPasswordConfirmation->text();

    if (pass.isEmpty() || confirm.isEmpty()) {
        QMessageBox::warning(this, "Fields Incomplete", "Both password verification blocks must be populated.");
        return;
    }

    if (pass != confirm) {
        QMessageBox::critical(this, "Mismatch Detected", "The typed password tokens do not match. Reverify muscle memory entry.");
        return;
    }

    QMessageBox::StandardButton midAirCheck = QMessageBox::warning(this,
                                                                   "⚠️ DANGER ZONE INTERLOCK",
                                                                   "ATTENTION: You are about to initiate firmware modification pipelines.\n\n"
                                                                   "Are you absolutely certain you have memorized this passphrase signature? If the bootloader rejects it, you will lose signature validation tracking access.",
                                                                   QMessageBox::Ok | QMessageBox::Abort,
                                                                   QMessageBox::Abort
    );

    if (midAirCheck != QMessageBox::Ok) {
        return;
    }

    QMessageBox::StandardButton finalGate = QMessageBox::question(this,
                                                                  "🔄 FINAL COMPLIANCE VERIFICATION",
                                                                  "Are you sure?\n\nSelect YES to write data to NVRAM lanes, or NO to review inputs.",
                                                                  QMessageBox::Yes | QMessageBox::No,
                                                                  QMessageBox::No
    );

    if (finalGate == QMessageBox::Yes) {
        this->accept();
    }
}

QString MokEnrollmentWizard::enforceSystemEnrollment(QWidget *parent)
{
    MokEnrollmentWizard wizard(parent);
    if (wizard.exec() == QDialog::Accepted) {
        return wizard.editUserPassword->text();
    }
    return QString();
}
