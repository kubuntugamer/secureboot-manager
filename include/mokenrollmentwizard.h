#ifndef MOKENROLLMENTWIZARD_H
#define MOKENROLLMENTWIZARD_H

#include <QDialog>
#include <QString>

class QLineEdit;
class QPushButton;
class QLabel;

class MokEnrollmentWizard : public QDialog
{
    Q_OBJECT
public:
    explicit MokEnrollmentWizard(QWidget *parent = nullptr);
    ~MokEnrollmentWizard() override = default;

    // Static runtime invocation utility rule that returns the user's defined password string
    static QString enforceSystemEnrollment(QWidget *parent);

private slots:
    void handleRegistrationSequence();

private:
    QLineEdit *editUserPassword = nullptr;
    QLineEdit *editPasswordConfirmation = nullptr;
    QPushButton *btnProceedToSystemImport = nullptr;
    QPushButton *btnAbortSafely = nullptr;
    QLabel *warningBanner = nullptr;

    void buildInterfaceElements();
};

#endif // MOKENROLLMENTWIZARD_H
