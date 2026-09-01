#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QLineEdit;
class QSpinBox;
class QPushButton;
class QTextBrowser;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // UI elements managed via generation modules
    QLineEdit *edit_gen_common_name;
    QSpinBox *spin_gen_days;
    QPushButton *btn_generate_mok;
    QTextBrowser *text_generation_log;
};

#endif // MAINWINDOW_H
