#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mokledgerpage.h"
#include "mokgeneratorpage.h"
#include "moksignerpage.h"
#include "mokaboutdialog.h"
#include "mokselectiontracker.h"
#include "mokprovider.h"
#include "mokdashboardconsole.h" // 📦 Dedicated console layout engine
#include <QHeaderView>
#include <QTableWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 📐 DIMENSION RESTORATION: Locked tightly to your production layout standards
    this->setFixedSize(1100, 700);

    // 🛡️ WINDOW ICON INJECTION: Overrides the Wayland compositor fallback graphic with your custom look
    // Attempts to load your custom logo path, falls back to a system-wide secure token icon if missing
    QIcon appIcon(":/images/shield.png");
    if (appIcon.isNull()) {
        appIcon = QIcon::fromTheme("security-high", QIcon::fromTheme("password-manager"));
    }
    this->setWindowIcon(appIcon);

    // 🎨 NAV BALANCE: Scaled down from 22x22 to 18x18 to reclaim vertical runway for bottom spacing
    ui->listWidget->setIconSize(QSize(18, 18));

    // 🚫 SCROLLBAR FORCE BARRIER: Explicitly override and block scrollbar injection policies
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 🛠️ SUBMODULE ENGINE INTERACTION: Delegate console styling and boot briefs out of this file
    dashboardConsole = new MokDashboardConsole(ui->text_raw_certificate_, this);
    dashboardConsole->renderStartupBrief();

    // 🧼 Purge static visual designer placeholders from the main right canvas stack view
    while (ui->stackedWidget->count() > 0) {
        ui->stackedWidget->removeWidget(ui->stackedWidget->widget(0));
    }

    // 🗺️ THE ROADMAP: Host your dynamic, adaptive department widgets natively inside the clean stack
    MokLedgerPage *ledgerPage = new MokLedgerPage(this);
    ui->stackedWidget->addWidget(ledgerPage);
    ui->stackedWidget->addWidget(new MokGeneratorPage(this));
    ui->stackedWidget->addWidget(new MokSignerPage(this));
    ui->stackedWidget->setCurrentIndex(0);

    // 📐 GRID BALANCE: Enforce column layout rules without letting strings clip out of bounds
    QTableWidget *table = ledgerPage->getTableWidget();
    if (table && table->horizontalHeader()) {
        table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
        table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    }

    // 🎯 SIDEBAR LINKING HOOK: Route row selection event messages back up to the left sidebar display panel
    QVector<MokKeyEntry> activeKeys = MokProvider::getLiveKeys();
    MokSelectionTracker::initTracking(ledgerPage->getTableWidget(), ui->text_raw_certificate_, activeKeys);

    // 🎨 SYSTEM THEME MAP: Bind uniform desktop iconography directly to your sidebar items
    if (ui->listWidget->count() >= 3) {
        ui->listWidget->item(0)->setIcon(QIcon::fromTheme("view-list-details"));
        ui->listWidget->item(1)->setIcon(QIcon::fromTheme("configure"));
        ui->listWidget->item(2)->setIcon(QIcon::fromTheme("accessories-text-editor"));
    }

    // ➕ THE FOOTER: Append the About item. Reclaimed runway creates a natural padding gap below this text.
    QListWidgetItem *aboutItem = new QListWidgetItem(QIcon::fromTheme("help-about"), "About Manager");
    aboutItem->setTextAlignment(Qt::AlignCenter);
    ui->listWidget->addItem(aboutItem);

    // 🕹️ NAVIGATION CONTROLLER: Connect panel index selection directly to the stacked window views
    connect(ui->listWidget, &QListWidget::currentRowChanged, this, [this](int rowIndex) {
        if (rowIndex == 3) {
            ui->listWidget->setCurrentRow(ui->stackedWidget->currentIndex());
            MokAboutDialog aboutWindow(this);
            aboutWindow.exec();
            return;
        }
        ui->stackedWidget->setCurrentIndex(rowIndex);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
