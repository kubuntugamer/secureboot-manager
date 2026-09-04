#ifndef MOKSIGNERPAGE_H
#define MOKSIGNERPAGE_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class AbstractDiscoveryEngine; // Forward declaration for our plug interface

class MokSignerPage : public QWidget
{
    Q_OBJECT
public:
    explicit MokSignerPage(QWidget *parent = nullptr);

private:
    QLineEdit *editSignTargetPath = nullptr;
    QLineEdit *editSignKeyPath = nullptr;
    QPushButton *btnBrowseBinary = nullptr;
    QPushButton *btnExecuteSignature = nullptr;

    AbstractDiscoveryEngine *discoveryEngine = nullptr; // Tracks the active plugin reference

    void setupExecutionHook();
    void triggerScan();
};

#endif // MOKSIGNERPAGE_H
