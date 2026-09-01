#include "mokgenerator.h"
#include <QProcess>
#include <QStringList>
#include <QDir>

MokGenerator::MokGenerator(QObject *parent) : QObject(parent) {}

bool MokGenerator::generateKeyPair(const QString &commonName, int days, int keySize, const QString &outputDir, QString &logOutput) {
    // 1. Sanity check: Ensure our destination directory layout physically exists on disk
    QDir dir(outputDir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            logOutput = "Error: Failed to create target output directory paths.";
            return false;
        }
    }

    QString keyPath = dir.absoluteFilePath("MOK.key");
    QString derPath = dir.absoluteFilePath("MOK.der");

    // 2. Map parameters out cleanly to a standard OpenSSL command parameter block
    QStringList arguments;
    arguments << "req" << "-new" << "-x509"
    << "-newkey" << QString("rsa:%1").arg(keySize) // ✨ FIX: Clean, native Qt6 string formatting!
    << "-nodes"
    << "-days" << QString::number(days)
    << "-keyout" << keyPath
    << "-out" << derPath
    << "-subj" << QString("/CN=%1/").arg(commonName);

    logOutput += "Executing string: openssl " + arguments.join(" ") + "\n\n";

    // 3. Establish an isolated process tracker to fire off the generation call rootless
    QProcess process;
    process.start("openssl", arguments);

    if (!process.waitForStarted() || !process.waitForFinished()) {
        logOutput += "Error: OpenSSL process failed to execute or timed out.";
        return false;
    }

    // 4. Collect terminal diagnostics strings to stream straight to your visual console box
    QString stdErr = QString::fromUtf8(process.readAllStandardError());
    QString stdOut = QString::fromUtf8(process.readAllStandardOutput());

    logOutput += stdErr + "\n" + stdOut;

    if (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0) {
        logOutput += "\n✨ Success! Key pair generated perfectly inside your target folder directory.";
        return true;
    }

    logOutput += "\n❌ Error: OpenSSL exited with failure code: " + QString::number(process.exitCode());
    return false;
}
