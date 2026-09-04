#ifndef MOKENROLLMENTWIZARD_H
#define MOKENROLLMENTWIZARD_H

#include <QDialog>
#include <QString>

class QCheckBox;
class QLineEdit;
class QPushButton;

class MokEnrollmentWizard : public QDialog
{
    Q_OBJECT
public:
    explicit MokEnrollmentWizard(QWidget *parent = nullptr);
    ~MokEnrollmentWizard() override = default;

    // Static runtime invocation utility rule that returns the user's defined password string
    static QString enforceSystemEnrollment(QWidget *parent);

private:
    QLineEdit *editUserPassword = nullptr;
    QLineEdit *editPasswordConfirmation = nullptr;
    QCheckBox *chkAcknowledgeBlueScreen = nullptr;
    QCheckBox *chkAcknowledgeDataLockout = nullptr;
    QPushButton *btnProceedToSystemImport = nullptr;
    QPushButton *btnAbortSafely = nullptr;

    void buildInterfaceElements();
    void evaluateInterlockState();
};

#endif // MOKENROLLMENTWIZARD_H
