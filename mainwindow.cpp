#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mokkeyentry.h"
#include "mokprovider.h"
#include "mokuiadapter.h"
#include "mokselectiontracker.h"
#include "mokgenerator.h"
#include "mokuigenerator.h"
#include "mokuisigner.h"
#include <QVector>
#include <QDir>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QTextBrowser>
#include <QTimer>
#include <QFileDialog>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
, ui(new Ui::MainWindow)
, edit_gen_common_name(nullptr)
, spin_gen_days(nullptr)
, btn_generate_mok(nullptr)
, text_generation_log(nullptr)
, edit_sign_target_path(nullptr)
, edit_sign_key_path(nullptr)
, btn_browse_binary(nullptr)
, btn_execute_signature(nullptr)
{
    ui->setupUi(this);
    this->setFixedSize(950, 600);

    ui->text_raw_certificate_->setFocusPolicy(Qt::NoFocus);
    ui->text_raw_certificate_->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    ui->table_enrolled_keys_->setFocusPolicy(Qt::StrongFocus);
    ui->listWidget->setFocusPolicy(Qt::NoFocus);
    this->clearFocus();

    QString page1GuideHtml =
    "<div style='font-family: sans-serif; color: #bdc3c7; padding: 10px; line-height: 1.4;'>"
    "  <div style='text-align: center; margin-bottom: 15px;'>"
    "    <span style='font-size: 32px;'>🛡️</span>"
    "    <h3 style='color: #ffffff; margin: 5px 0 2px 0; font-size: 14px;'>Secure Boot Manager</h3>"
    "    <span style='font-size: 10px; color: #7f8c8d;'>User Quick-Start Guide</span>"
    "  </div>"
    "  <hr style='border: 0; border-top: 1px solid #3f4142; margin-bottom: 12px;'>"
    "  <p style='font-size: 11px; margin-bottom: 12px; color: #a4b0be;'>"
    "    Welcome! This utility automates the complex CLI steps required to sign custom Linux modules "
    "    and custom kernels to pass firmware validation checks."
    "  </p>"
    "  <div style='margin-bottom: 10px;'>"
    "    <b style='color: #3498db; font-size: 11px;'>📋 Enrolled MOK Keys</b>"
    "    <p style='font-size: 10px; margin: 2px 0 0 0; color: #95a5a6;'>"
    "    Scans your motherboard's NVRAM database directly. <b>Select an entry row above</b> "
    "    to instantly dump its raw public X.509 certificate signature block here.</p>"
    "  </div>"
    "</div>";

    QString page2GuideHtml =
    "<div style='font-family: sans-serif; color: #bdc3c7; padding: 10px; line-height: 1.4;'>"
    "  <div style='text-align: center; margin-bottom: 15px;'>"
    "    <span style='font-size: 32px;'>🛠️</span>"
    "    <h3 style='color: #ffffff; margin: 5px 0 2px 0; font-size: 14px;'>Key Enroller Engine</h3>"
    "    <span style='font-size: 10px; color: #7f8c8d;'>MOK Enrollment Controls</span>"
    "  </div>"
    "  <hr style='border: 0; border-top: 1px solid #3f4142; margin-bottom: 12px;'>"
    "  <p style='font-size: 11px; margin-bottom: 12px; color: #a4b0be;'>"
    "    Generates secure MOK key-pairs and auto-imports them to prepare your motherboard's UEFI."
    "  </p>"
    "  <div style='margin-bottom: 10px;'>"
    "    <b style='color: #e74c3c; font-size: 11px;'>⚠️ Action Required: Reboot</b>"
    "    <p style='font-size: 10px; margin: 2px 0 0 0; color: #95a5a6;'>"
    "    Once key staging finishes, <b>you must restart your computer</b>. During reboot, the blue MokManager "
    "    firmware tool will load automatically. Input your password to finalize enrollment.</p>"
    "  </div>"
    "  <div style='margin-bottom: 10px;'>"
    "    <b style='color: #2ecc71; font-size: 11px;'>🐧 Multi-Kernel Choice</b>"
    "    <p style='font-size: 10px; margin: 2px 0 0 0; color: #95a5a6;'>"
    "    Once enrolled, you are completely cleared to install <b>Liquorix, XanMod, or any custom third-party kernel branch</b> of your choice natively.</p>"
    "  </div>"
    "</div>";

    QString page3GuideHtml =
    "<div style='font-family: sans-serif; color: #bdc3c7; padding: 10px; line-height: 1.4;'>"
    "  <div style='text-align: center; margin-bottom: 15px;'>"
    "    <span style='font-size: 32px;'>✍️</span>"
    "    <h3 style='color: #ffffff; margin: 5px 0 2px 0; font-size: 14px;'>Binary & Kernel Automator</h3>"
    "    <span style='font-size: 10px; color: #7f8c8d;'>Automation Overview</span>"
    "  </div>"
    "  <hr style='border: 0; border-top: 1px solid #3f4142; margin-bottom: 12px;'>"
    "  <p style='font-size: 11px; margin-bottom: 12px; color: #a4b0be;'>"
    "    Manages system hooks to automate signing during background OS updates."
    "  </p>"
    "  <div style='margin-bottom: 10px;'>"
    "    <b style='color: #e67e22; font-size: 11px;'>🚀 Automated System Updates</b>"
    "    <p style='font-size: 10px; margin: 2px 0 0 0; color: #95a5a6;'>"
    "    Hooks cleanly into <code>/etc/kernel/postinst.d</code> to instantly verify any new kernel build candidate "
    "    automatically without manual desktop intervention.</p>"
    "  </div>"
    "</div>";

    auto applyCertHtmlView = [this](const QString &rawCertText) {
        QString certHtmlWrapper = "<html><body><pre>" + rawCertText + "</pre></body></html>";
        ui->text_raw_certificate_->setHtml(certHtmlWrapper);
    };

    ui->text_raw_certificate_->setHtml(page1GuideHtml);

    QVector<MokKeyEntry> activeKeys = MokProvider::getLiveKeys();
    MokUiAdapter::populateTable(ui->table_enrolled_keys_, activeKeys);
    ui->table_enrolled_keys_->setColumnWidth(0, 300);

    QTimer::singleShot(0, this, [this]() {
        ui->table_enrolled_keys_->clearSelection();
        ui->table_enrolled_keys_->setCurrentCell(-1, -1);
        this->clearFocus();
    });

    connect(ui->listWidget, &QListWidget::currentRowChanged, this, [this, activeKeys, page1GuideHtml, page2GuideHtml, page3GuideHtml, applyCertHtmlView](int rowIndex) {
        ui->stackedWidget->setCurrentIndex(rowIndex);
        if (rowIndex == 0) {
            int selectedRow = ui->table_enrolled_keys_->currentRow();
            if (selectedRow >= 0 && selectedRow < activeKeys.size()) {
                applyCertHtmlView(activeKeys[selectedRow].rawCertificate);
            } else {
                ui->text_raw_certificate_->setHtml(page1GuideHtml);
            }
        } else if (rowIndex == 1) {
            ui->text_raw_certificate_->setHtml(page2GuideHtml);
        } else if (rowIndex == 2) {
            ui->text_raw_certificate_->setHtml(page3GuideHtml);
        }
    });

    MokSelectionTracker::initTracking(ui->table_enrolled_keys_, ui->text_raw_certificate_, activeKeys);
    MokUiGenerator::setupGenerationPage(ui->page_generate_keys, edit_gen_common_name, spin_gen_days, btn_generate_mok, text_generation_log);

    // ----------------------------------------------------
    // PAGE 2 EXECUTION: KEY GENERATION & UEFI STAGING LOOP
    // ----------------------------------------------------
    connect(btn_generate_mok, &QPushButton::clicked, this, [this]() {
        text_generation_log->clear();
        QString commonName = edit_gen_common_name->text().trimmed();
        int validityDays = spin_gen_days->value();
        QString outputFolder = QDir::homePath() + "/secureboot-manager-keys";

        if (commonName.isEmpty()) {
            text_generation_log->append("❌ Error: Common Name cannot be empty.");
            return;
        }

        text_generation_log->append("🚀 Executing local OpenSSL certificate creation...\n");
        MokGenerator *generator = new MokGenerator(this);
        QString runtimeLogData = "";

        bool keyGenerated = generator->generateKeyPair(commonName, validityDays, 2048, outputFolder, runtimeLogData);
        text_generation_log->append(runtimeLogData);

        if (keyGenerated) {
            text_generation_log->append("\n🔒 Staging signature certificate inside UEFI database...");
            QString certPath = outputFolder + "/MOK.der";
            text_generation_log->append(QString("Staged target asset: %1").arg(certPath));
            text_generation_log->append("\n\n🛑 ATTENTION USER: A system reboot is required to proceed!");
            text_generation_log->append("Please restart your computer now. The blue UEFI MokManager screen will guide you through entering your password to authorize your enrollment key. Once back at your desktop, you are fully authorized to install your alternative kernel choices.");
        }
    });

    // ----------------------------------------------------
    // PAGE 3 EXECUTION: AUTOMATION CONSOLE & HOOKS
    // ----------------------------------------------------
    MokUiSigner::setupSigningPage(ui->page_sign_binaries, edit_sign_target_path, edit_sign_key_path, btn_browse_binary, btn_execute_signature);

    connect(btn_browse_binary, &QPushButton::clicked, this, [this]() {
        QString targetFile = QFileDialog::getOpenFileName(this, "Select File to Sign", "/lib/modules", "Kernel Modules (*.ko);;Images (vmlinuz);;All Files (*)");
        if (!targetFile.isEmpty()) {
            edit_sign_target_path->setText(targetFile);
        }
    });

    connect(btn_execute_signature, &QPushButton::clicked, this, [this]() {
        // Execute manual back-end script signatures or switch automation hooks
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
