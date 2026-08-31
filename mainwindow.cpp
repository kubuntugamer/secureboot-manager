#include "mainwindow.h"
#include "ui_mainwindow.h"

// --- Include our Unix-philosophy submodules ---
#include "mokkeyentry.h"
#include "mokmockprovider.h"
#include "mokuiadapter.h"
#include "mokselectiontracker.h"

#include <QVector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. Fetch our data arrays from our dedicated provider submodule
    QVector<MokKeyEntry> activeKeys = MokMockProvider::getMockKeys();

    // 2. Pass data strings to your layout mapping adapter submodule to populate the grid
    MokUiAdapter::populateTable(ui->table_enrolled_keys_, activeKeys);

    // 3. Activate your selection tracking submodule to handle information box updates
    MokSelectionTracker::initTracking(ui->table_enrolled_keys_, ui->text_raw_certificate_, activeKeys);
    ui->table_enrolled_keys_->setColumnWidth(0, 300);
}

MainWindow::~MainWindow()
{
    delete ui;
}
