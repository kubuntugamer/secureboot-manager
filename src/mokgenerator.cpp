#include "mokgenerator.h"
#include <QProcess>
#include <QDir>
#include <QFile>

MokGenerator::MokGenerator(QObject *parent)
: QObject(parent)
{
}

// 🔒 UPDATED SIGNATURE: Accepts baseFileName to ensure precise workspace file localization
bool MokGenerator::generateKeyPair(const QString &commonName, const QString &baseFileName, int days, int keySize, const QString &outputDir, QString &logOutput)
{
    QDir targetDir(outputDir);
    if (!targetDir.exists()) {
        if (!targetDir.mkpath(".")) {
            logOutput += "[ERROR] Failed to instantiate target staging folder path: " + outputDir + "\n";
            return false;
        }
    }

    // 🎯 FIX: Tracks files using the unique base identity string instead of static 'MOK.*' tokens
    QString privateKeyPath = targetDir.absoluteFilePath(QString("%1.priv").arg(baseFileName));
    QString publicKeyPath = targetDir.absoluteFilePath(QString("%1.der").arg(baseFileName));

    if (QFile::exists(privateKeyPath)) QFile::remove(privateKeyPath);
    if (QFile::exists(publicKeyPath)) QFile::remove(publicKeyPath);

    QString program = "openssl";
    QStringList arguments;
    arguments << "req" << "-new" << "-x509"
    << "-newkey" << QString("rsa:%1").arg(keySize)
    << "-nodes"
    << "-days" << QString::number(days)
    << "-keyout" << privateKeyPath
    << "-out" << publicKeyPath
    << "-subj" << QString("/CN=%1/").arg(commonName)
    << "-outform" << "DER";

    logOutput += "[EXEC] Running command: openssl " + arguments.join(" ") + "\n";

    QProcess process;
    process.start(program, arguments);

    if (!process.waitForFinished(10000)) {
        process.kill();
        logOutput += "[FATAL] OpenSSL process execution timed out or was forcefully blocked.\n";
        return false;
    }

    QString stdOut = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
    QString stdErr = QString::fromUtf8(process.readAllStandardError()).trimmed();

    if (!stdOut.isEmpty()) logOutput += stdOut + "\n";
    if (!stdErr.isEmpty()) logOutput += stdErr + "\n";

    if (process.exitCode() == 0 && QFile::exists(privateKeyPath) && QFile::exists(publicKeyPath)) {
        logOutput += "[SUCCESS] Cryptographic key signatures written cleanly to file system array channels.\n";
        return true;
    }

    logOutput += QString("[ERROR] OpenSSL generation pipeline failed with exit code: %1\n").arg(process.exitCode());
    return false;
}
