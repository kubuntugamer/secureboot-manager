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

    // 🏢 SUBMODULE GROUP 1 & 2 INITIALIZATION: Set window boundaries and paint navigation drawer shapes
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

    // 🛡️ DYNAMIC SYSTEM ICON BINDING: Queries the active icon theme set by the user's KDE settings.
    // If the active icon theme lacks this specific fallback hook, it gracefully grabs standard alternative keys.
    QIcon dynamicSystemIcon = QIcon::fromTheme("preferences-security",
                                               QIcon::fromTheme("security-high",
                                                                QIcon::fromTheme("password-manager")));

    this->setWindowIcon(dynamicSystemIcon);
}

void MainWindow::setupNavigationSidebar()
{
    ui->listWidget->setIconSize(QSize(22, 22)); // Set to 22x22 to scale perfectly with KDE default themes
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 🎨 ICON THEME ALIGNMENT: Uses standard Freedesktop naming keys that automatically
    // adapt shape, color, and stroke style based on the user's active icon pack (Breeze, Papirus, etc.)
    if (ui->listWidget->count() >= 3) {
        ui->listWidget->item(0)->setIcon(QIcon::fromTheme("view-list-details", QIcon::fromTheme("list-ordered")));
        ui->listWidget->item(1)->setIcon(QIcon::fromTheme("preferences-system-session", QIcon::fromTheme("configure")));
        ui->listWidget->item(2)->setIcon(QIcon::fromTheme("draw-text", QIcon::fromTheme("accessories-text-editor")));
    }
}

void MainWindow::initializePageStack()
{
    dashboardConsole = new MokDashboardConsole(ui->text_raw_certificate_, this);
    dashboardConsole->renderStartupBrief();

    // 🧹 WIPE DESIGNER PLACEHOLDERS: Destroys original mockup canvas wrappers to prevent layer overlap
    while (ui->stackedWidget->count() > 0) {
        QWidget *stalePage = ui->stackedWidget->widget(0);
        ui->stackedWidget->removeWidget(stalePage);
        stalePage->deleteLater();
    }

    // 🔒 UNIFIED DATA ACQUISITION: Pull system and staged keys exactly once to prevent cache splits
    QVector<MokKeyEntry> activeKeys = MokProvider::getLiveKeys();

    // 🎨 SUBMODULE GROUP 2 CALL: Pass the unified data reference straight into the ledger page constructor
    MokLedgerPage *ledgerPage = new MokLedgerPage(activeKeys, this);
    ui->stackedWidget->addWidget(ledgerPage);                 // Custom Index 0
    ui->stackedWidget->addWidget(new MokGeneratorPage(this));     // Custom Index 1
    ui->stackedWidget->addWidget(new MokSignerPage(this));        // Custom Index 2
    ui->stackedWidget->setCurrentIndex(0);

    // 🔒 SAFE POINTER EXTRACTION: Capture the persistent table handle directly from the live runtime instance
    QTableWidget *table = ledgerPage->getTableWidget();

    // 🔍 TRACKING ALIGNMENT: Feed that exact same memory reference straight to the tracker engine
    if (table) {
        MokSelectionTracker::initTracking(table, ui->text_raw_certificate_, activeKeys);

        // 🧼 CLEAN STARTUP STATE: Clear initial cell focuses so the table boots unselected
        table->clearSelection();
        table->setCurrentItem(nullptr);
    }
}

void MainWindow::setupNavigationController()
{
    // 🎛️ CONTROLLER BINDINGS: Maps sidebar item click streams onto the stacked widget indices safely
    connect(ui->listWidget, &QListWidget::currentRowChanged, this, [this](int rowIndex) {
        if (rowIndex < 0) return;

        // If About Manager is selected, open the dialog cleanly without causing recursive signal loops
        if (rowIndex == 3) {
            MokAboutDialog aboutWindow(this);
            aboutWindow.exec();

            // Quietly revert the sidebar highlight frame index back onto the true current container view
            ui->listWidget->blockSignals(true);
            ui->listWidget->setCurrentRow(ui->stackedWidget->currentIndex());
            ui->listWidget->blockSignals(false);
            return;
        }

        ui->stackedWidget->setCurrentIndex(rowIndex);

        // 📡 BRIEFING CONTROLLERS: Update side info panels dynamically based on active page changes
        if (rowIndex == 0) {
            dashboardConsole->renderStartupBrief();
        } else if (rowIndex == 1) {
            dashboardConsole->renderGenerationBrief();
        } else if (rowIndex == 2) {
            dashboardConsole->renderSigningBrief();
        }
    });

    // Ensure the dynamically added About Manager item uses the correct native icon theme hook as well
    if (ui->listWidget->count() == 3) {
        QListWidgetItem *aboutItem = new QListWidgetItem(
            QIcon::fromTheme("help-about", QIcon::fromTheme("dialog-information")),
                                                         "About Manager"
        );
        aboutItem->setTextAlignment(Qt::AlignCenter);
        ui->listWidget->addItem(aboutItem);
    }
}
