#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// 📦 Forward declaration to keep compilation fast and lean
class MokDashboardConsole;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    MokDashboardConsole *dashboardConsole; // ➕ Allocated pointer link for the console submodule
};

#endif // MAINWINDOW_H
