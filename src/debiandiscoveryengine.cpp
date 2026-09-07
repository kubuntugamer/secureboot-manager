#include "debiandiscoveryengine.h"
#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QApplication>

DebianDiscoveryEngine::DebianDiscoveryEngine(QObject *parent) : AbstractDiscoveryEngine(parent) {}

void DebianDiscoveryEngine::discoverKernels()
{
    // FIX: Use pointer variables inside local heap allocations to store state context across async slots
    auto *uNames = new QStringList();
    auto *uPaths = new QStringList();
    auto *sNames = new QStringList();

    QDir bootDir("/boot");
    QStringList kernelFiles = bootDir.entryList(QStringList() << "vmlinuz-*", QDir::Files);

    if (kernelFiles.isEmpty()) {
        emit discoveryFinished(*uNames, *uPaths, *sNames);
        // Clean up context containers to prevent data leaks
        delete uNames; delete uPaths; delete sNames;
        return;
    }

    // FIX: Allocate the process context to the heap to ensure layout boundaries remain active
    QProcess *elevatedScanner = new QProcess(this);

    // Filter file listings before structuring execution arrays to bypass ghost assets early
    QStringList validKernels;
    for (const QString &kernelFile : kernelFiles) {
        if (kernelFile.startsWith(".") || kernelFile.endsWith(".unsigned") ||
            kernelFile.endsWith(".bak")  || kernelFile.contains(".tmp_sign") ||
            kernelFile.endsWith(".tmp")) {
            continue;
            }
            validKernels.append(kernelFile);
    }

    if (validKernels.isEmpty()) {
        emit discoveryFinished(*uNames, *uPaths, *sNames);
        delete uNames; delete uPaths; delete sNames;
        elevatedScanner->deleteLater();
        return;
    }

    QString shellPayload = "";
    for (int i = 0; i < validKernels.size(); ++i) {
        QString absolutePath = bootDir.filePath(validKernels.at(i));
        shellPayload += QString("echo '=== KERNEL_INDEX_%1 ==='; sbverify --list '%2' 2>&1; ").arg(i).arg(absolutePath);
    }

    // FIX: Concurrently link finishing states to the async dispatcher—completely removing waitForFinished UI blocks
    connect(elevatedScanner, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, elevatedScanner, validKernels, bootDir, uNames, uPaths, sNames](int exitCode, QProcess::ExitStatus status) {

                if (status == QProcess::NormalExit && exitCode == 0) {
                    QString consoleOutput = QString::fromUtf8(elevatedScanner->readAllStandardOutput());

                    for (int i = 0; i < validKernels.size(); ++i) {
                        QString kernelFile = validKernels.at(i);
                        QString absolutePath = bootDir.filePath(kernelFile);
                        QString version = kernelFile.mid(8);

                        QString blockMarker = QString("=== KERNEL_INDEX_%1 ===").arg(i);
                        int markerIdx = consoleOutput.indexOf(blockMarker);

                        bool cryptographicPass = false;
                        if (markerIdx != -1) {
                            int nextMarkerIdx = consoleOutput.indexOf(QString("=== KERNEL_INDEX_%1 ===").arg(i + 1));
                            QString blockText = (nextMarkerIdx != -1)
                            ? consoleOutput.mid(markerIdx, nextMarkerIdx - markerIdx)
                            : consoleOutput.mid(markerIdx);

                            if (blockText.contains("signature") && !blockText.contains("No signature table present")) {
                                cryptographicPass = true;
                            }
                        }

                        if (cryptographicPass) {
                            sNames->append(version);
                        } else {
                            uNames->append(version);
                            uPaths->append(absolutePath);
                        }
                    }
                } else {
                    qWarning() << "⚠️ Async backend core discovery processing layer faulted or closed prematurely.";
                }

                // Emit our populated data models downstream safely
                emit discoveryFinished(*uNames, *uPaths, *sNames);

                // Clear dynamic memory footprints from the system stack cleanly
                delete uNames; delete uPaths; delete sNames;
                elevatedScanner->deleteLater();
            });

    // Start the root escalation prompt loop asynchronously
    elevatedScanner->start("pkexec", QStringList() << "sh" << "-c" << shellPayload);
}

void DebianDiscoveryEngine::processNextPackage(const QStringList &packages, int index,
                                               QStringList uNames, QStringList uPaths, QStringList sNames,
                                               const QStringList &signedCachePaths)
{
    Q_UNUSED(packages);
    Q_UNUSED(index);
    Q_UNUSED(uNames);
    Q_UNUSED(uPaths);
    Q_UNUSED(sNames);
    Q_UNUSED(signedCachePaths);
}
