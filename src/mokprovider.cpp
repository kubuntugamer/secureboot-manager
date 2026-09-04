#include "mokprovider.h"
#include <QProcess>
#include <QTextStream>
#include <QDebug>

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

    // 3. Robust tokenizer loop re-mapped to parse true native mokutil console lines
    while (stream.readLineInto(&line)) {
        QString trimmedLine = line.trimmed();

        // Each unique UEFI firmware key block always starts with the Fingerprint statement label
        if (trimmedLine.startsWith("SHA1 Fingerprint:") || trimmedLine.startsWith("SHA-1 Fingerprint:")) {
            if (parsingKey) {
                currentKey.rawCertificate = currentRawBlock.trimmed();
                liveCache.append(currentKey);
            }
            // Initialize fresh cache values for the newly discovered hardware index row entry
            currentKey = MokKeyEntry();
            currentKey.commonName = "Unknown Certificate Owner";
            currentKey.expirationDate = "N/A";
            currentKey.serialNumber = "N/A";
            currentRawBlock = line + "\n";
            parsingKey = true;
        } else if (parsingKey) {
            currentRawBlock += line + "\n";

            // Parse identity label text fields using Subj/Issuer constraints natively
            if ((trimmedLine.contains("Subj:") || trimmedLine.contains("Issuer:")) && trimmedLine.contains("CN=")) {
                int cnStart = trimmedLine.indexOf("CN=") + 3;
                int cnEnd = trimmedLine.indexOf(",", cnStart);

                // If there are no trailing organizational commas, slice to the absolute end of the string
                if (cnEnd == -1) {
                    currentKey.commonName = trimmedLine.mid(cnStart).trimmed();
                } else {
                    currentKey.commonName = trimmedLine.mid(cnStart, cnEnd - cnStart).trimmed();
                }

                // Clean off any lingering raw formatting escape characters if present
                if (currentKey.commonName.startsWith('"') && currentKey.commonName.endsWith('"')) {
                    currentKey.commonName = currentKey.commonName.mid(1, currentKey.commonName.length() - 2);
                }
            }
            // 🛠️ FIXED TIMESTAMP SLICER: Slice cleanly right after the first header colon mapping matching index
            else if (trimmedLine.contains("Not After :") || trimmedLine.contains("Not After:")) {
                int sepIndex = trimmedLine.indexOf(":");
                if (sepIndex != -1) {
                    currentKey.expirationDate = trimmedLine.mid(sepIndex + 1).trimmed();
                }
            }
            // Capture serial number listings safely
            else if (trimmedLine.contains("Serial Number:") || trimmedLine.contains("Serial:")) {
                int sepIndex = trimmedLine.indexOf(":");
                QString potentialSerial = trimmedLine.mid(sepIndex + 1).trimmed();

                if (!potentialSerial.isEmpty()) {
                    currentKey.serialNumber = potentialSerial;
                } else {
                    stream.readLineInto(&line);
                    currentRawBlock += line + "\n";
                    currentKey.serialNumber = line.trimmed();
                }
            }
        }
    }

    // Append the final token bracket block resting inside the parser stream registers safely
    if (parsingKey) {
        currentKey.rawCertificate = currentRawBlock.trimmed();
        liveCache.append(currentKey);
    }

    qDebug() << "📊 MokProvider parsed live firmware keys count:" << liveCache.size();
    return liveCache;
}
