#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mokkeyentry.h"
#include "mokprovider.h"
#include "mokuiadapter.h"
#include "mokselectiontracker.h"
#include "mokgenerator.h"
#include "mokuigenerator.h"
#include <QVector>
#include <QDir>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QTextBrowser>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
, ui(new Ui::MainWindow)
, edit_gen_common_name(nullptr)
, spin_gen_days(nullptr)
, btn_generate_mok(nullptr)
, text_generation_log(nullptr)
{
    ui->setupUi(this);

    // 🔒 BRUTE-FORCE FIXED SIZE LOCKOUT: Hard-locks the OS frame boundaries to exactly 950x600.
    this->setFixedSize(950, 600);

    // 🛑 STAGE 1 FOCUS LOCKOUT: Completely strip focus handling from the text container.
    // This allows text highlight and copy/paste shortcuts but prevents Qt from shifting focus here at boot.
    ui->text_raw_certificate_->setFocusPolicy(Qt::NoFocus);

    // Explicitly configure text interaction behavior: fully selectable and copyable, but strictly read-only.
    ui->text_raw_certificate_->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

    // 🛑 STAGE 2 FOCUS LOCKOUT: Ensure table rows are not automatically selected or focused on boot.
    ui->table_enrolled_keys_->setFocusPolicy(Qt::NoFocus);
    ui->table_enrolled_keys_->clearSelection();

    // 🛑 STAGE 3 FOCUS LOCKOUT: Divert the initial layout focus destination away from interactive fields entirely.
    ui->listWidget->setFocusPolicy(Qt::NoFocus);
    this->clearFocus();

    // ----------------------------------------------------
    // QUICK-START HELP GUIDES (PRE-RENDERED BLOCKS)
    // ----------------------------------------------------
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
    "    and third-party kernels (like Liquorix) to pass firmware validation checks."
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
    "    <h3 style='color: #ffffff; margin: 5px 0 2px 0; font-size: 14px;'>Key Generation Engine</h3>"
    "    <span style='font-size: 10px; color: #7f8c8d;'>Crypto Setup Instructions</span>"
    "  </div>"
    "  <hr style='border: 0; border-top: 1px solid #3f4142; margin-bottom: 12px;'>"
    "  <p style='font-size: 11px; margin-bottom: 12px; color: #a4b0be;'>"
    "    This interface automates secure, rootless OpenSSL backend streams to establish independent ownership rules."
    "  </p>"
    "  <div style='margin-bottom: 10px;'>"
    "    <b style='color: #2ecc71; font-size: 11px;'>⚙️ Parameter Tuning</b>"
    "    <p style='font-size: 10px; margin: 2px 0 0 0; color: #95a5a6;'>"
    "    Define a recognizable unique identity name string. The validation length threshold is set to a "
    "    pre-locked, developer-grade 100-year security limit (36,500 days) to prevent certificate timeouts.</p>"
    "  </div>"
    "</div>";

    QString page3GuideHtml =
    "<div style='font-family: sans-serif; color: #bdc3c7; padding: 10px; line-height: 1.4;'>"
    "  <div style='text-align: center; margin-bottom: 15px;'>"
    "    <span style='font-size: 32px;'>✍️</span>"
    "    <h3 style='color: #ffffff; margin: 5px 0 2px 0; font-size: 14px;'>Binary Module Signer</h3>"
    "    <span style='font-size: 10px; color: #7f8c8d;'>Execution Instructions</span>"
    "  </div>"
    "  <hr style='border: 0; border-top: 1px solid #3f4142; margin-bottom: 12px;'>"
    "  <p style='font-size: 11px; margin-bottom: 12px; color: #a4b0be;'>"
    "    A streamlined graphical wrapper layer over your running kernel's integrated target hash tools."
    "  </p>"
    "  <div style='margin-bottom: 10px;'>"
    "    <b style='color: #e67e22; font-size: 11px;'>🚀 Local Modules</b>"
    "    <p style='font-size: 10px; margin: 2px 0 0 0; color: #95a5a6;'>"
    "    Injects verified user signatures natively into out-of-tree hardware drivers (<code>.ko</code> extension files) "
    "    and fresh uncompressed <code>vmlinuz</code> system kernel partitions directly without dropping back down to bash.</p>"
    "  </div>"
    "</div>";

    // Set initial boot text context layout cleanly
    ui->text_raw_certificate_->setHtml(page1GuideHtml);

    // ----------------------------------------------------
    // PAGE 1 INITIALIZATION (Enrolled MOK Keys Grid Populate)
    // ----------------------------------------------------
    QVector<MokKeyEntry> activeKeys = MokProvider::getLiveKeys();
    MokUiAdapter::populateTable(ui->table_enrolled_keys_, activeKeys);
    ui->table_enrolled_keys_->setColumnWidth(0, 300);

    // ----------------------------------------------------
    // SIDEBAR NAVIGATION ROUTER (PERMANENT VISIBILITY)
    // ----------------------------------------------------
    connect(ui->listWidget, &QListWidget::currentRowChanged, this, [this, page1GuideHtml, page2GuideHtml, page3GuideHtml](int rowIndex) {
        ui->stackedWidget->setCurrentIndex(rowIndex);

        // Clear active key selections when bouncing between categories to refresh layouts cleanly
        ui->table_enrolled_keys_->clearSelection();

        // Dynamically rotate guide text inside the panel without destroying widget visibility
        if (rowIndex == 0) {
            ui->text_raw_certificate_->setHtml(page1GuideHtml);
        } else if (rowIndex == 1) {
            ui->text_raw_certificate_->setHtml(page2GuideHtml);
        } else if (rowIndex == 2) {
            ui->text_raw_certificate_->setHtml(page3GuideHtml);
        }
    });

    // ----------------------------------------------------
    // LIVE SELECTION TRACKING OVERRIDE LAYER
    // ----------------------------------------------------
    MokSelectionTracker::initTracking(ui->table_enrolled_keys_, ui->text_raw_certificate_, activeKeys);

    // ----------------------------------------------------
    // PAGE 2 INITIALIZATION (Automated Code Key Generation)
    // ----------------------------------------------------
    MokUiGenerator::setupGenerationPage(ui->page_generate_keys, edit_gen_common_name, spin_gen_days, btn_generate_mok, text_generation_log);

    connect(btn_generate_mok, &QPushButton::clicked, this, [this]() {
        text_generation_log->clear();
        text_generation_log->append("🚀 Initializing OpenSSL generation pipeline protocol...\n");

        QString commonName = edit_gen_common_name->text().trimmed();
        int validityDays = spin_gen_days->value();
        int rsaKeySize = 2048;
        QString outputFolder = QDir::homePath() + "/secureboot-manager-keys";

        if (commonName.isEmpty()) {
            text_generation_log->append("❌ Halted: Common Name field cannot be empty.");
            return;
        }

        MokGenerator *generator = new MokGenerator(this);
        QString runtimeLogData = "";

        text_generation_log->append(QString("Target Export Directory: %1\n").arg(outputFolder));

        bool executionSuccess = generator->generateKeyPair(commonName, validityDays, rsaKeySize, outputFolder, runtimeLogData);
        text_generation_log->append(runtimeLogData);

        if (executionSuccess) {
            text_generation_log->append("\n📂 Backup Checkpoint: Files successfully written locally!");
        }
    });
}

MainWindow::~MainWindow() {
    delete ui;
}
