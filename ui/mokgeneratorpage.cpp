#include "mokgeneratorpage.h"
#include "mokgenerator.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QTextBrowser>
#include <QLabel>
#include <QMessageBox>

MokGeneratorPage::MokGeneratorPage(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    editGenCommonName = new QLineEdit(this);
    editGenCommonName->setPlaceholderText("e.g., Custom Secure Boot Module Key");

    spinGenDays = new QSpinBox(this);
    spinGenDays->setRange(1, 3650);
    spinGenDays->setValue(365);

    formLayout->addRow("Common Name (CN):", editGenCommonName);
    formLayout->addRow("Validity (Days):", spinGenDays);
    mainLayout->addLayout(formLayout);

    btnGenerateMok = new QPushButton("Generate Custom MOK Key Pair", this);
    btnGenerateMok->setStyleSheet(
        "QPushButton {"
        "  background-color: #2a3b4d;"
        "  color: #3daee9;"
        "  border: 1px solid #3daee9;"
        "  font-weight: bold;"
        "  padding: 8px;"
        "  border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #3daee9;"
        "  color: #ffffff;"
        "}"
    );
    mainLayout->addWidget(btnGenerateMok);

    textGenerationLog = new QTextBrowser(this);
    textGenerationLog->setStyleSheet(
        "background-color: #121214;"
        "color: #1dd1a1;"
        "font-family: monospace;"
        "border: 1px solid #3f4142;"
    );
    mainLayout->addWidget(new QLabel("Execution Diagnostics Log Terminal:"));
    mainLayout->addWidget(textGenerationLog);

    connect(btnGenerateMok, &QPushButton::clicked, this, [this]() {
        QString commonName = editGenCommonName->text().trimmed();
        int lifespanDays = spinGenDays->value();
        int defaultKeySize = 2048;
        QString outputDirectory = "./secureboot-manager-keys";

        if (commonName.isEmpty()) {
            QMessageBox::warning(this, "Validation Failed", "Common Name signature tracking label cannot be blank.");
            return;
        }

        textGenerationLog->clear();
        textGenerationLog->append("--- INITIALIZING CRYPTOGRAPHIC PROCESS ENVIRONMENT ---\n");

        QString executionLogs = "";
        MokGenerator generatorEngine;
        bool success = generatorEngine.generateKeyPair(commonName, lifespanDays, defaultKeySize, outputDirectory, executionLogs);

        textGenerationLog->append(executionLogs);

        if (success) {
            QMessageBox::information(this, "Success", "Custom cryptographic MOK pair written to disk successfully!");
        } else {
            QMessageBox::critical(this, "Generation Failed", "OpenSSL routine returned a critical subprocess validation failure.");
        }
    });
}
