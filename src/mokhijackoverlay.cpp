#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProcess>
#include <QKeyEvent>

// Custom Subclass to completely block the user from closing the lock overlay via keyboard escapes
class ImmovableLockWindow : public QDialog {
public:
    explicit ImmovableLockWindow(QWidget *parent) : QDialog(parent) {}
protected:
    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Escape) {
            event->accept(); // Swallow escape keystroke event strings to prevent closing
        } else {
            QDialog::keyPressEvent(event);
        }
    }
    void reject() override {
        // Intercept close events to lock the widget container canvas down permanently
    }
};

void launchBlockadeRebootOverlay(QWidget *parent)
{
    ImmovableLockWindow *lockWindow = new ImmovableLockWindow(parent);
    lockWindow->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
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

    // ⏳ Disruptive Postponement Trigger
    QObject::connect(btnRebootLater, &QPushButton::clicked, lockWindow, [lockWindow, bigAlert, infoText, btnRebootLater]() {
        // Drop full-screen boundaries so they can access their native Linux desktop workspace safely
        lockWindow->showNormal();
        lockWindow->setFixedSize(500, 320);

        // Repaint the screen panel elements to look like a persistent warning alert block card
        lockWindow->setWindowTitle("⚠️ Pending System Restart");
        bigAlert->setText("⚠️ SECURE BOOT INCOMPLETE");
        bigAlert->setStyleSheet("font-size: 14px; font-weight: bold; color: #f39c12; text-align: center;");

        infoText->setText("Your core development application window remains locked down.\n\n"
        "You can minimize this dialog card wrapper, but you must restart your machine "
        "before any secure manager settings can be altered or running states changed.");

        // Eliminate the duplicate button layout step out of the floating block state view canvas
        btnRebootLater->hide();
    });
}
