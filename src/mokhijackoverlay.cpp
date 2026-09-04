#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProcess>

void launchBlockadeRebootOverlay(QWidget *parent)
{
    QDialog *lockWindow = new QDialog(parent, Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    lockWindow->setModal(true);
    lockWindow->setStyleSheet("background-color: #0c0d10; color: #ffffff; font-family: monospace;");
    lockWindow->showFullScreen();

    QVBoxLayout *box = new QVBoxLayout(lockWindow);
    box->setAlignment(Qt::AlignCenter);
    box->setSpacing(25);

    QLabel *bigAlert = new QLabel("⚠️ SYSTEM PENDING PRE-BOOT ENROLLMENT FIRMWARE STATE CACHED", lockWindow);
    bigAlert->setStyleSheet("font-size: 16px; font-weight: bold; color: #e74c3c; text-align: center;");
    box->addWidget(bigAlert);

    QLabel *infoText = new QLabel(
        "The machine owner keys have been flagged for kernel registration.\n\n"
        "To protect system integrity, this workspace layout is locked until a host refresh occurs.\n"
        "You must restart the system to enter the MOK interface management engine.",
        lockWindow
    );
    infoText->setWordWrap(true);
    infoText->setStyleSheet("font-size: 11px; color: #95a5a6; text-align: center; line-height: 1.5; max-width: 600px;");
    box->addWidget(infoText);

    // 🔘 Control Layout for Choice Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(15);

    QPushButton *btnRebootSystemNow = new QPushButton("REBOOT COMPUTER IMMEDIATELY", lockWindow);
    btnRebootSystemNow->setStyleSheet("background-color: #e74c3c; color: #ffffff; font-weight: bold; padding: 15px 30px; border-radius: 4px; font-size: 11px;");

    QPushButton *btnRebootLater = new QPushButton("I Will Reboot Manually Later", lockWindow);
    btnRebootLater->setStyleSheet("background-color: #2c3e50; color: #bdc3c7; font-weight: bold; padding: 15px 30px; border-radius: 4px; font-size: 11px; border: 1px solid #3f4142;");

    btnLayout->addWidget(btnRebootSystemNow);
    btnLayout->addWidget(btnRebootLater);
    box->addLayout(btnLayout);

    // 🚀 Immediate Restart Trigger
    QObject::connect(btnRebootSystemNow, &QPushButton::clicked, lockWindow, []() {
        QProcess::startDetached("systemctl", QStringList() << "reboot");
    });

    // ⏳ Manual Postponement Trigger
    QObject::connect(btnRebootLater, &QPushButton::clicked, lockWindow, [lockWindow]() {
        // Keeps the workspace layout screen blocked, but does not execute a hardware force restart
        lockWindow->setWindowTitle("⚠️ Pending System Restart");
    });
}
