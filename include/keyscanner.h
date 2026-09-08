#ifndef KEYSCANNER_H
#define KEYSCANNER_H

#include "moksignerpage.h"
#include <QString>
#include <QStringList>
#include <QList>

class KeyScanner {
public:
    static QList<KeyProfile> scanForKeys();
    static QString extractCommonName(const QString &certPath);

private:
    static QString findMatchingCertificate(const QString &dirPath, const QString &baseName);
};

#endif // KEYSCANNER_H
