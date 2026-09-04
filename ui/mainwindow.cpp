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
    this->setFixedSize(1100, 700);

    QIcon appIcon(":/images/shield.png");
    if (appIcon.isNull()) {
        appIcon = QIcon::fromTheme("security-high", QIcon::fromTheme("password-manager"));
    }
    this->setWindowIcon(appIcon);
}

void MainWindow::setupNavigationSidebar()
{
    ui->listWidget->setIconSize(QSize(18, 18));
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    if (ui->listWidget->count() >= 3) {
        ui->listWidget->item(0)->setIcon(QIcon::fromTheme("view-list-details"));
        ui->listWidget->item(1)->setIcon(QIcon::fromTheme("configure"));
        ui->listWidget->item(2)->setIcon(QIcon::fromTheme("accessories-text-editor"));
    }

    QListWidgetItem *aboutItem = new QListWidgetItem(QIcon::fromTheme("help-about"), "About Manager");
    aboutItem->setTextAlignment(Qt::AlignCenter);
    ui->listWidget->addItem(aboutItem);
}

void MainWindow::initializePageStack()
{
    dashboardConsole = new MokDashboardConsole(ui->text_raw_certificate_, this);
    dashboardConsole->renderStartupBrief();

    // 🧹 WIPE ALL ORIGINAL DESIGNER WIDGETS COMPLETELY OUT OF THE STACK ENGINE
    // This stops old, duplicate .ui placeholder layouts from intercepting our view screens!
    while (ui->stackedWidget->count() > 0) {
        QWidget *stalePage = ui->stackedWidget->widget(0);
        ui->stackedWidget->removeWidget(stalePage);
        stalePage->deleteLater();
    }

    // 🔒 REGISTER ONLY OUR REAL CUSTOM IMPLEMENTED CODE PAGES
    MokLedgerPage *ledgerPage = new MokLedgerPage(this);
    ui->stackedWidget->addWidget(ledgerPage);             // Index 0
    ui->stackedWidget->addWidget(new MokGeneratorPage(this)); // Index 1
    ui->stackedWidget->addWidget(new MokSignerPage(this));    // Index 2
    ui->stackedWidget->setCurrentIndex(0);

    // Grid Stretching Mechanics for the Central Ledger
    QTableWidget *table = ui->table_enrolled_keys_;
    if (!table) {
        table = ledgerPage->getTableWidget();
    }

    if (table && table->horizontalHeader()) {
        table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
        table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
        table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        table->setColumnWidth(0, 280);
        table->setColumnWidth(1, 200);
    }

    QVector<MokKeyEntry> activeKeys = MokProvider::getLiveKeys();
    MokSelectionTracker::initTracking(table, ui->text_raw_certificate_, activeKeys);
}

void MainWindow::setupNavigationController()
{
    connect(ui->listWidget, &QListWidget::currentRowChanged, this, [this](int rowIndex) {
        if (rowIndex == 3) {
            ui->listWidget->setCurrentRow(ui->stackedWidget->currentIndex());
            MokAboutDialog aboutWindow(this);
            aboutWindow.exec();
            return;
        }
        ui->stackedWidget->setCurrentIndex(rowIndex);

        if (rowIndex == 0) {
            dashboardConsole->renderStartupBrief();
        } else if (rowIndex == 1) {
            dashboardConsole->renderGenerationBrief();
        } else if (rowIndex == 2) {
            dashboardConsole->renderSigningBrief();
        }
    });
}
