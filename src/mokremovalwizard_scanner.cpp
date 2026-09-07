#include "mokremovalwizard.h"
#include <QDir>
#include <QProcess>

void MokRemovalWizard::scanForSignedBinaries()
{
    // FIX: Explicitly clear target buffers instantly upon entry to isolate dynamic repeated selections
    affectedKernels.clear();
    processingKernelIndex = 0;

    QDir bootDir("/boot");
    QStringList kernelFiles = bootDir.entryList(QStringList() << "vmlinuz-*", QDir::Files);

    for (const QString &kernel : kernelFiles) {
        // FIX: Inject explicit defensive rules to bypass intermediate staging leak files
        if (kernel.startsWith(".") || kernel.endsWith(".unsigned") ||
            kernel.endsWith(".bak")  || kernel.contains(".tmp_sign") ||
            kernel.endsWith(".tmp")) {
            continue;
            }

            QString kernelPath = bootDir.absoluteFilePath(kernel);

        QProcess verifyProcess;
        verifyProcess.start("sbverify", QStringList() << "--list" << kernelPath);
        if (!verifyProcess.waitForFinished(2000)) continue;

        QString output = QString::fromUtf8(verifyProcess.readAllStandardOutput());

        // Match system identities against both common name labels or hardware serial tags securely
        if (output.contains(targetKeyName) || (!targetKeySerial.isEmpty() && output.contains(targetKeySerial))) {
            affectedKernels << kernelPath;
        }
    }
}
