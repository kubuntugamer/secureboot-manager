#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mokledgerpage.h"
#include "mokgeneratorpage.h"
#include "moksignerpage.h"
#include "mokaboutdialog.h"
#include "mokselectiontracker.h"
#include "mokprovider.h"
#include "mokdashboardconsole.h"
#include <QHeaderView>
#include <QTableWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 🎨 The Head Chef calls the four sous-chefs to action
    setupWindowProperties();
    setupNavigationSidebar();
    initializePageStack();
    setupNavigationController();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupWindowProperties()
{
    // Sets the physical size layout standard to 1100x700
    this->setFixedSize(1100, 700);

    // Injects the custom security shield graphic into the window frame
    QIcon appIcon(":/images/shield.png");
    if (appIcon.isNull()) {
        appIcon = QIcon::fromTheme("security-high", QIcon::fromTheme("password-manager"));
    }
    this->setWindowIcon(appIcon);
}

void MainWindow::setupNavigationSidebar()
{
    // Scales sidebar icons nicely and hides the ugly scrollbars
    ui->listWidget->setIconSize(QSize(18, 18));
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Maps out default icons to your left menu elements
    if (ui->listWidget->count() >= 3) {
        ui->listWidget->item(0)->setIcon(QIcon::fromTheme("view-list-details"));
        ui->listWidget->item(1)->setIcon(QIcon::fromTheme("configure"));
        ui->listWidget->item(2)->setIcon(QIcon::fromTheme("accessories-text-editor"));
    }

    // Appends the clean "About Manager" item to the bottom row
    QListWidgetItem *aboutItem = new QListWidgetItem(QIcon::fromTheme("help-about"), "About Manager");
    aboutItem->setTextAlignment(Qt::AlignCenter);
    ui->listWidget->addItem(aboutItem);
}

void MainWindow::initializePageStack()
{
    // Wires up the dashboard side panel display
    dashboardConsole = new MokDashboardConsole(ui->text_raw_certificate_, this);
    dashboardConsole->renderStartupBrief();

    // 🔒 THE HIDE STRATEGY: Safely turn off the original designer placeholder layout views
    if (ui->page_generate_keys) ui->page_generate_keys->hide();
    if (ui->page_sign_binaries)  ui->page_sign_binaries->hide();

    // Wipes out any leftover layout indexes inside the stack engine
    while (ui->stackedWidget->count() > 0) {
        ui->stackedWidget->removeWidget(ui->stackedWidget->widget(0));
    }

    // Loads your real workspace modules (Ledger, Builder, Signer panels)
    MokLedgerPage *ledgerPage = new MokLedgerPage(this);
    ui->stackedWidget->addWidget(ledgerPage);
    ui->stackedWidget->addWidget(new MokGeneratorPage(this));
    ui->stackedWidget->addWidget(new MokSignerPage(this));
    ui->stackedWidget->setCurrentIndex(0);

    // 📐 FIXED STARTUP GRID STRETCH: Query the central table layout directly to expand its width bounds
    QTableWidget *table = ui->table_enrolled_keys_;
    if (!table) {
        // Fallback check: look for the table nested inside the custom page object
        table = ledgerPage->getTableWidget();
    }

    if (table && table->horizontalHeader()) {
        // Enforce specific layout widths to prevent the "nmon Name (C" string clipping glitch
        table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
        table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
        table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

        // Give the Common Name column a generous, safe fixed runway width on first load
        table->setColumnWidth(0, 280);
        table->setColumnWidth(1, 200);
    }

    // Links your row selections back into the console information engine
    QVector<MokKeyEntry> activeKeys = MokProvider::getLiveKeys();
    MokSelectionTracker::initTracking(table, ui->text_raw_certificate_, activeKeys);
}

void MainWindow::setupNavigationController()
{
    // Listens to your click selections and switches screens instantly
    connect(ui->listWidget, &QListWidget::currentRowChanged, this, [this](int rowIndex) {
        if (rowIndex == 3) {
            ui->listWidget->setCurrentRow(ui->stackedWidget->currentIndex());
            MokAboutDialog aboutWindow(this);
            aboutWindow.exec();
            return;
        }
        ui->stackedWidget->setCurrentIndex(rowIndex);

        // Updates side panel text layouts relative to the active screen index
        if (rowIndex == 0) {
            dashboardConsole->renderStartupBrief();
        } else if (rowIndex == 1) {
            dashboardConsole->renderGenerationBrief();
        } else if (rowIndex == 2) {
            dashboardConsole->renderSigningBrief();
        }
    });
}
