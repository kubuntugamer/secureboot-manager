#include "mokuisigner.h"
#include <QProcess>
#include <QFile>
#include <QDebug>

bool MokUiSigner::unsignKernelBinary(const QString &kernelPath)
{
    if (kernelPath.isEmpty() || !QFile::exists(kernelPath)) {
        qWarning() << "Invalid path:" << kernelPath;
        return false;
    }
    QStringList arguments;
    arguments << "sbattach" << "--remove" << kernelPath;
    QProcess process;
    process.start("pkexec", arguments);
    if (!process.waitForStarted(3000)) return false;
    process.waitForFinished(-1);
    return (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0);
}
