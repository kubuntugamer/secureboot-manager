#include "keyscanner.h"
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

QList<KeyProfile> KeyScanner::scanForKeys()
{
    QList<KeyProfile> profiles;
    QStringList searchDirs = {
        QDir::homePath() + QStringLiteral("/secureboot-manager-keys/"),
        QStringLiteral("/var/lib/shim-signed/mok/")
    };

    for (const QString &dirPath : searchDirs) {
        QDir dir(dirPath);
        if (!dir.exists()) continue;

        // Find keys by looking for private key file extensions
        QStringList filters = { QStringLiteral("*.priv"), QStringLiteral("*.key") };
        QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);

        for (const QFileInfo &fileInfo : fileList) {
            QString keyPath = fileInfo.absoluteFilePath();
            QString baseName = fileInfo.baseName();
            QString certPath = findMatchingCertificate(dirPath, baseName);

            KeyProfile profile;
            profile.privateKeyPath = keyPath;
            profile.certificatePath = certPath;
            profile.originPath = dirPath;

            // Generate clean human-readable titles
            QString resolvedTitle = extractCommonName(certPath);
            if (resolvedTitle.isEmpty()) {
                resolvedTitle = baseName;
                resolvedTitle.replace(QStringLiteral("_"), QStringLiteral(" "));
                resolvedTitle.replace(QStringLiteral("-"), QStringLiteral(" "));
            }

            if (certPath.isEmpty()) {
                profile.displayName = resolvedTitle + QStringLiteral(" ⚠️ (Missing Certificate)");
            } else if (dirPath.contains(QStringLiteral("/var/lib/"))) {
                profile.displayName = resolvedTitle + QStringLiteral(" [System Default]");
            } else {
                profile.displayName = resolvedTitle + QStringLiteral(" [User Key]");
            }

            profiles.append(profile);
        }
    }
    return profiles;
}

QString KeyScanner::extractCommonName(const QString &certPath)
{
    if (certPath.isEmpty() || !QFile::exists(certPath)) return QString();

    QProcess process;
    QStringList arguments;
    arguments << QStringLiteral("x509") << QStringLiteral("-in") << certPath
    << QStringLiteral("-noout") << QStringLiteral("-subject");

    if (certPath.endsWith(QStringLiteral(".der"), Qt::CaseInsensitive)) {
        arguments << QStringLiteral("-inform") << QStringLiteral("DER");
    } else {
        arguments << QStringLiteral("-inform") << QStringLiteral("PEM");
    }

    process.start(QStringLiteral("/usr/bin/openssl"), arguments);
    if (!process.waitForStarted(1500) || !process.waitForFinished(3000)) return QString();

    QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
    QRegularExpression cnRegex(QStringLiteral("(?:CN\\s*=\\s*|\\/CN\\s*=)([^/,\n]+)"));
    QRegularExpressionMatch match = cnRegex.match(output);

    if (match.hasMatch()) {
        QString parsedName = match.captured(1).trimmed();
        parsedName.remove(QStringLiteral("\""));
        parsedName.remove(QStringLiteral("'"));
        return parsedName;
    }
    return QString();
}

QString KeyScanner::findMatchingCertificate(const QString &dirPath, const QString &baseName)
{
    QStringList exactExtensions = { QStringLiteral(".pem"), QStringLiteral(".crt"), QStringLiteral(".der") };
    for (const QString &ext : exactExtensions) {
        QString fullCertPath = dirPath + baseName + ext;
        if (QFileInfo::exists(fullCertPath)) return fullCertPath;
    }
    return QString();
}
