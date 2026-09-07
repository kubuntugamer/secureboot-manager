#ifndef MOKREMOVALWIZARD_H
#define MOKREMOVALWIZARD_H

#include <QDialog>
#include <QStringList>

class QLineEdit;
class QPushButton;
class QLabel;
class QStackedWidget;
class QTextBrowser;
class QListWidget; // 🔍 Visual Anchor: Forward declare list widget components

class MokRemovalWizard : public QDialog {
    Q_OBJECT

public:
    explicit MokRemovalWizard(const QString &keyName, const QString &keySerial, QWidget *parent = nullptr);
    ~MokRemovalWizard() override = default; //

    // =========================================================================
    // 🔓 SOVEREIGNTY RESTORED: NON-MODAL LIFETIME INTERLOCK
    // ASSOCIATED CODE FILES: include/mokremovalwizard.h, src/mokremovalwizard_init.cpp
    // ROADMAP STEP: Pass raw pointer allocations to let windows float non-modally
    // =========================================================================
    static MokRemovalWizard* executeKeyRevocation(const QString &keyName, const QString &keySerial, QWidget *parent); //

private slots:
    void handleRemediationPipeline(); //
    void executeMokNVRAMPurge(); //
    void unsignNextKernel(); // Moved to slots for clean callback accessibility

    // FIX: Declared unified routing slot to clear the "no declaration matches" Ninja build error
    void onRemediationActionRouteTriggered();

private:
    QString targetKeyName; //
    QString targetKeySerial; //
    QStringList affectedKernels; //
    int processingKernelIndex = 0; //
    bool isEnvironmentValid = true; //

    QStackedWidget *wizardStack = nullptr; //
    QListWidget *keyListWidget = nullptr; // 🔒 Data Model Tracking: Maps selected rows to active targets
    QTextBrowser *logTerminal = nullptr; //
    QPushButton *btnStartRemediation = nullptr; //
    QPushButton *btnCancelRemediation = nullptr; //

    QLineEdit *editResetPassword = nullptr; //
    QLineEdit *editResetConfirm = nullptr; //
    QPushButton *btnExecutePurge = nullptr; //

    bool verifySystemToolchain(); //
    void scanForSignedBinaries(); //
    void buildSelectionPage(); // 🛠️ Stage 1 Blueprint Hook: Declares initial ledger list panel
    void buildRemediationPage(); //
    void buildPasswordAssignmentPage(); //

    // FIX: Declared the Liquorix APT signing hook cleanup utility
    void neutralizeAptSigningHooks();
};

#endif // MOKREMOVALWIZARD_H
