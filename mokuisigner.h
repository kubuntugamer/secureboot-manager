#ifndef MOKUISIGNER_H
#define MOKUISIGNER_H

class QWidget;
class QLineEdit;
class QPushButton;

class MokUiSigner
{
public:
    static void setupSigningPage(QWidget *pageContainer,
                                 QLineEdit *&targetPathEdit,
                                 QLineEdit *&keyPathEdit,
                                 QPushButton *&browseBinaryBtn,
                                 QPushButton *&executeSignBtn);
};

#endif // MOKUISIGNER_H
