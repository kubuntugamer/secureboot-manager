#ifndef MOKSIGNERPAGE_H
#define MOKSIGNERPAGE_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class AbstractDiscoveryEngine;

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

    AbstractDiscoveryEngine *discoveryEngine = nullptr;

    void setupExecutionHook();
    void triggerScan();
};

#endif // MOKSIGNERPAGE_H
