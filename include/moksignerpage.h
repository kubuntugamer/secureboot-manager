#ifndef MOKSIGNERPAGE_H
#define MOKSIGNERPAGE_H

#include <QWidget>

class QLineEdit;
class QPushButton;

class MokSignerPage : public QWidget
{
    Q_OBJECT
public:
    explicit MokSignerPage(QWidget *parent = nullptr);

private:
    QLineEdit *editSignTargetPath;
    QLineEdit *editSignKeyPath;
    QPushButton *btnBrowseBinary;
    QPushButton *btnExecuteSignature;
};

#endif // MOKSIGNERPAGE_H
