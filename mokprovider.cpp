#include "mokprovider.h"
#include <QProcess>
#include <QTextStream>

QVector<MokKeyEntry> MokProvider::getLiveKeys() {
    QVector<MokKeyEntry> liveCache;

    // 1. Establish an isolated QProcess channel to execute the real system command rootless
    QProcess sbsignProcess;
    sbsignProcess.start("mokutil", QStringList() << "--list-enrolled");

    if (!sbsignProcess.waitForStarted() || !sbsignProcess.waitForFinished()) {
        // Fallback safety layer: returns an empty tracker if command isn't available
        return liveCache;
    }

    // 2. Stream the raw output bytes into a readable string block
    QString output = QString::fromUtf8(sbsignProcess.readAllStandardOutput());
    QTextStream stream(&output);
    QString line;

    MokKeyEntry currentKey;
    bool parsingKey = false;
    QString currentRawBlock = "";

    // 3. Clean single-pass tokenizer string scanner routine parsing individual parameters
    while (stream.readLineInto(&line)) {
        if (line.startsWith("[key ")) {
            if (parsingKey) {
                currentKey.rawCertificate = currentRawBlock.trimmed();
                liveCache.append(currentKey);
            }
            // Reset cache fields for next key tracking cycle
            currentKey = MokKeyEntry();
            currentKey.commonName = "Unknown Certificate Owner";
            currentKey.expirationDate = "N/A";
            currentKey.serialNumber = "N/A";
            currentRawBlock = line + "\n";
            parsingKey = true;
        } else if (parsingKey) {
            currentRawBlock += line + "\n";

            // Extract Common Name from Issuer/Subject lines
            if (line.contains("Issuer:") && line.contains("CN=")) {
                int cnStart = line.indexOf("CN=") + 3;
                int cnEnd = line.indexOf(",", cnStart);
                if (cnEnd == -1) {
                    currentKey.commonName = line.mid(cnStart).trimmed();
                } else {
                    currentKey.commonName = line.mid(cnStart, cnEnd - cnStart).trimmed();
                }
            }
            // Isolate validity dates
            else if (line.contains("Not After :")) {
                currentKey.expirationDate = line.split("Not After :").last().trimmed();
            }
            // Extract the Hex string Serial Identification line variables
            else if (line.contains("Serial Number:")) {
                stream.readLineInto(&line);
                currentRawBlock += line + "\n";
                currentKey.serialNumber = line.trimmed();
            }
        }
    }

    // Append the final token bracket block resting inside the parser stream registers
    if (parsingKey) {
        currentKey.rawCertificate = currentRawBlock.trimmed();
        liveCache.append(currentKey);
    }

    return liveCache;
}
