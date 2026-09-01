#ifndef MOKGENERATOR_H
#define MOKGENERATOR_H

#include <QString>
#include <QObject>

class MokGenerator : public QObject {
    Q_OBJECT

public:
    explicit MokGenerator(QObject *parent = nullptr);

    // Single responsibility: Fired up to run rootless OpenSSL key pair creation
    bool generateKeyPair(const QString &commonName, int days, int keySize, const QString &outputDir, QString &logOutput);
};

#endif // MOKGENERATOR_H
