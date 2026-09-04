#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MokDashboardConsole;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // 🛠️ The Blueprints: We list the names of our four new sous-chef tasks here
    void setupWindowProperties();
    void setupNavigationSidebar();
    void initializePageStack();
    void setupNavigationController();

    Ui::MainWindow *ui;
    MokDashboardConsole *dashboardConsole;
};

#endif // MAINWINDOW_H
