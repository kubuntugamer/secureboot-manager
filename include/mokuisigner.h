#ifndef MOKUISIGNER_H
#define MOKUISIGNER_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QStringList>

class MokUiSigner {
public:
    static void setupSigningPage(QWidget *pageContainer,
                                 QLineEdit *&targetPathEdit,
                                 QLineEdit *&keyPathEdit,
                                 QPushButton *&browseBinaryBtn,
                                 QPushButton *&executeSignBtn,
                                 const QStringList &unsignedLabels = QStringList(),
                                 const QStringList &unsignedPaths = QStringList(),
                                 const QStringList &signedLabels = QStringList(),
                                 const QStringList &signedPaths = QStringList());

    static bool unsignKernelBinary(const QString &kernelPath);
};

#endif // MOKUISIGNER_H
