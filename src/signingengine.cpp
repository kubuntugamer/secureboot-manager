#include "signingengine.h"
#include <QApplication>

SigningEngine::SigningEngine(QObject *parent) : QObject(parent) {}

void SigningEngine::injectSignature(const QString &binaryPath, const QString &keyPath, const QString &certPath)
{
    emit logMessageReceived(QStringLiteral("🚀 [SYSTEM INIT]: Launching structural signing pipeline..."));
    emit logMessageReceived(QStringLiteral("📂 [TARGET]: ") + binaryPath);

    QProcess *worker = new QProcess(this);
    QStringList arguments;
    arguments << QStringLiteral("sbsign") << QStringLiteral("--key") << keyPath
    << QStringLiteral("--cert") << certPath << QStringLiteral("--output") << binaryPath << binaryPath;

    connect(worker, &QProcess::readyReadStandardOutput, this, [=]() {
        emit logMessageReceived(QString::fromUtf8(worker->readAllStandardOutput()).trimmed());
    });
    connect(worker, &QProcess::readyReadStandardError, this, [=]() {
        emit logMessageReceived(QStringLiteral("<font color='#e74c3c'>⚠️ </font>") + QString::fromUtf8(worker->readAllStandardError()).trimmed());
    });

    connect(worker, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int code, QProcess::ExitStatus status) {
        if (status == QProcess::NormalExit && code == 0) {
            emit logMessageReceived(QStringLiteral("<font color='#1dd1a1'>✨ [SUCCESS]: Binary signed successfully.</font>"));
            runBootloaderRefresh();
        } else {
            emit logMessageReceived(QStringLiteral("<font color='#e74c3c'>❌ [FAILURE]: Signing failed.</font>"));
            emit operationFinished(false, QStringLiteral("The signing process reported errors."));
        }
        worker->deleteLater();
    });

    worker->start(QStringLiteral("/usr/bin/pkexec"), arguments);
}

void SigningEngine::stripSignature(const QString &binaryPath)
{
    emit logMessageReceived(QStringLiteral("⚠️ [SYSTEM INIT]: Launching kernel signature removal pipeline..."));

    QProcess *worker = new QProcess(this);
    QStringList arguments;
    arguments << QStringLiteral("sbattach") << QStringLiteral("--remove") << binaryPath;

    connect(worker, &QProcess::readyReadStandardOutput, this, [=]() {
        emit logMessageReceived(QString::fromUtf8(worker->readAllStandardOutput()).trimmed());
    });

    connect(worker, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int code, QProcess::ExitStatus status) {
        if (status == QProcess::NormalExit && code == 0) {
            emit logMessageReceived(QStringLiteral("<font color='#1dd1a1'>✨ [SUCCESS]: Signature block stripped clean.</font>"));
            runBootloaderRefresh();
        } else {
            emit operationFinished(false, QStringLiteral("The signature removal process failed."));
        }
        worker->deleteLater();
    });

    worker->start(QStringLiteral("/usr/bin/pkexec"), arguments);
}

void SigningEngine::runBootloaderRefresh()
{
    emit logMessageReceived(QStringLiteral("<font color='#3daee9'>⚙️ [BOOTLOADER]: Initializing GRUB partition refresh... Awaiting polkit pass.</font>"));

    QProcess *worker = new QProcess(this);
    connect(worker, &QProcess::readyReadStandardOutput, this, [=]() {
        emit logMessageReceived(QString::fromUtf8(worker->readAllStandardOutput()).trimmed());
    });

    connect(worker, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int code, QProcess::ExitStatus status) {
        if (status == QProcess::NormalExit && code == 0) {
            emit logMessageReceived(QStringLiteral("<font color='#1dd1a1'>✨ [SUCCESS]: GRUB configuration maps synchronized.</font>"));
            emit operationFinished(true, QStringLiteral("Operation successful and GRUB configuration updated."));
        } else {
            emit operationFinished(true, QStringLiteral("Operation completed but GRUB failed to refresh automatically."));
        }
        worker->deleteLater();
    });

    worker->start(QStringLiteral("/usr/bin/pkexec"), QStringList() << QStringLiteral("/usr/sbin/update-grub"));
    worker->waitForFinished(10000);
}
