#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mokkeyentry.h"
#include "mokprovider.h"           // 🔌 Targets our real live hardware provider
#include "mokuiadapter.h"
#include "mokselectiontracker.h"
#include <QVector>

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. Fetch data vectors directly from live system hardware layers rootless
    QVector<MokKeyEntry> activeKeys = MokProvider::getLiveKeys();

    // 2. Delegate data arrays to your existing UI adapter grid mapping routine
    MokUiAdapter::populateTable(ui->table_enrolled_keys_, activeKeys);

    // 3. Bind table highlighting to instant text browser updates
    MokSelectionTracker::initTracking(ui->table_enrolled_keys_, ui->text_raw_certificate_, activeKeys);

    // 4. Force isolated Column 0 size envelope to 300px
    ui->table_enrolled_keys_->setColumnWidth(0, 300);
}

MainWindow::~MainWindow()
{
    delete ui;
}
