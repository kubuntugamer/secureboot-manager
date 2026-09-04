#ifndef MOKKEYENTRY_H
#define MOKKEYENTRY_H

#include <QString>

struct MokKeyEntry {
    QString commonName;
    QString expirationDate;
    QString serialNumber;
    QString rawCertificate;
};

#endif // MOKKEYENTRY_H
