#ifndef MOKSIGNINGWIZARD_H
#define MOKSIGNINGWIZARD_H

#include <QDialog>
#include <QString>

class QStackedWidget;
class QPushButton;
class QTextBrowser;
class QLabel;
class QCheckBox;
class QComboBox; // 🔍 Added component definition for key dropdown selection entries

class MokSigningWizard : public QDialog
{
    Q_OBJECT
public:
    explicit MokSigningWizard(const QString &kernelVersion, QWidget *parent = nullptr);
    ~MokSigningWizard() override = default;

    static void executeSigningFlow(const QString &kernelVersion, QWidget *parent);

private slots:
    void processSignatureInjection();
    void finalizeAutomationDeployment();

private:
    QString targetKernelVersion;
    QString targetKernelPath;
    QString sandboxKeyDir;

    QStackedWidget *wizardStack = nullptr;

    // View Components Page 1
    QLabel *lblAssetVerification = nullptr;
    QComboBox *keySelectorComboBox = nullptr; // 🔑 Dynamic selector container
    QPushButton *btnStartSigning = nullptr;

    // View Components Page 2
    QTextBrowser *terminalLogView = nullptr;
    QPushButton *btnProceedToAutomation = nullptr;

    // View Components Page 3
    QCheckBox *chkInstallSystemHook = nullptr;
    QPushButton *btnCompleteWizard = nullptr;

    void buildVerificationView();
    void buildExecutionTerminalView();
    void buildAutomationHookView();
};

#endif // MOKSIGNINGWIZARD_H
