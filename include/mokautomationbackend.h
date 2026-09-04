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

    // Deploys the automated post-install hook script into /etc/kernel/postinst.d/
    bool deployKernelPostInstHook(const QString &keyPath, const QString &derPath, QString &logOutput);

    // Injects the custom key configuration paths into the global DKMS framework file
    bool configureDkmsAutomation(const QString &keyPath, const QString &derPath, QString &logOutput);
};

#endif // MOKAUTOMATIONBACKEND_H
