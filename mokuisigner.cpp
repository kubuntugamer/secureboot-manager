#include "mokuisigner.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

void MokUiSigner::setupSigningPage(QWidget *pageContainer, QLineEdit *&targetPathEdit, QLineEdit *&keyPathEdit, QPushButton *&browseBinaryBtn, QPushButton *&executeSignBtn)
{
    if (!pageContainer) return;

    QVBoxLayout *mainLayout = new QVBoxLayout(pageContainer);
    mainLayout->setContentsMargins(30, 25, 30, 25);
    mainLayout->setSpacing(15);

    // Page Section Title Headers
    QLabel *titleLabel = new QLabel("Automated Secure Boot Protection & Injections", pageContainer);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #ffffff;");
    mainLayout->addWidget(titleLabel);

    QLabel *subLabel = new QLabel("Configure automated background hooks to track and sign new system kernel releases or sign standalone third-party driver extensions manually.", pageContainer);
    subLabel->setStyleSheet("font-size: 11px; color: #bdc3c7;");
    subLabel->setWordWrap(true);
    mainLayout->addWidget(subLabel);

    mainLayout->addSpacing(10);

    // AUTOMATION HOOK CONFIGURATION REGION
    QLabel *autoHeader = new QLabel("Kernel Update Automation Guard Configuration:", pageContainer);
    autoHeader->setStyleSheet("font-size: 11px; font-weight: bold; color: #ffffff;");
    mainLayout->addWidget(autoHeader);

    QPushButton *btnEnableAuto = new QPushButton("🛡️ Enable Automatic Background Update Signing", pageContainer);
    btnEnableAuto->setStyleSheet("background-color: #27ae60; color: #ffffff; border: 1px solid #27ae60; border-radius: 4px; padding: 12px; font-size: 11px; font-weight: bold;");
    mainLayout->addWidget(btnEnableAuto);

    mainLayout->addSpacing(15);
    mainLayout->addWidget(new QLabel("<hr style='border: 0; border-top: 1px solid #3f4142;'>", pageContainer));
    mainLayout->addSpacing(5);

    // MANUAL BACKUP INJECTION REGION
    QLabel *manualHeader = new QLabel("Manual Single-Item Cryptographic Injections:", pageContainer);
    manualHeader->setStyleSheet("font-size: 11px; font-weight: bold; color: #e67e22;");
    mainLayout->addWidget(manualHeader);

    targetPathEdit = new QLineEdit(pageContainer);
    targetPathEdit->setPlaceholderText("/lib/modules/your-kernel-version/kernel/drivers/...");
    targetPathEdit->setStyleSheet("background-color: #1e1e24; color: #ffffff; border: 1px solid #3f4142; border-radius: 4px; padding: 6px; font-size: 11px;");

    browseBinaryBtn = new QPushButton("Browse File", pageContainer);
    browseBinaryBtn->setStyleSheet("background-color: #2c3e50; color: #ffffff; border: 1px solid #3f4142; border-radius: 4px; padding: 6px 12px; font-size: 11px; font-weight: bold;");

    QHBoxLayout *row = new QHBoxLayout();
    row->addWidget(targetPathEdit);
    row->addWidget(browseBinaryBtn);
    mainLayout->addLayout(row);

    keyPathEdit = new QLineEdit(pageContainer);
    keyPathEdit->setPlaceholderText("Defaults to active key: /home/chazz/secureboot-manager-keys/MOK.key");
    keyPathEdit->setStyleSheet("background-color: #1e1e24; color: #ffffff; border: 1px solid #3f4142; border-radius: 4px; padding: 6px; font-size: 11px;");
    mainLayout->addWidget(keyPathEdit);

    executeSignBtn = new QPushButton("Sign Standalone Target Binary Item", pageContainer);
    executeSignBtn->setStyleSheet("background-color: #d35400; color: #ffffff; border: 1px solid #d35400; border-radius: 4px; padding: 10px; font-size: 11px; font-weight: bold;");
    mainLayout->addWidget(executeSignBtn);

    mainLayout->addStretch();
}
