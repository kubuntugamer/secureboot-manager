#ifndef MOKAUTOMATIONBACKEND_H
#define MOKAUTOMATIONBACKEND_H

#include <QString>
#include <QObject>

class MokAutomationBackend : public QObject
{
    Q_OBJECT

public:
    explicit MokAutomationBackend(QObject *parent = nullptr);
    ~MokAutomationBackend();

    bool deployKernelPostInstHook(const QString &keyPath, const QString &derPath, QString &logOutput);
    bool configureDkmsAutomation(const QString &keyPath, const QString &derPath, QString &logOutput);

    // Explicitly registers our new module so the compiler stops throwing a 'not a member' error
    static QString generateSigningPayload(const QString &keyPath, const QString &certPath, const QString &targetBinary, const QString &kernelVersion);
};

#endif // MOKAUTOMATIONBACKEND_H
