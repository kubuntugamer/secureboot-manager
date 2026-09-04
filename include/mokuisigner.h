#ifndef MOKUISIGNER_H
#define MOKUISIGNER_H

#include <QStringList>

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
                                 QPushButton *&executeSignBtn,
                                 const QStringList &unsignedLabels,
                                 const QStringList &unsignedPaths,
                                 const QStringList &signedLabels);
};

#endif // MOKUISIGNER_H
