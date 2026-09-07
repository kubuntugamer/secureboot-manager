#ifndef MOKGENERATOR_H
#define MOKGENERATOR_H

#include <QString>
#include <QObject>

class MokGenerator : public QObject {
    Q_OBJECT

public:
    explicit MokGenerator(QObject *parent = nullptr);

    // 🔒 FIXED SIGNATURE: Added 'const QString &baseFileName' parameter to match engine rules
    bool generateKeyPair(const QString &commonName, const QString &baseFileName, int days, int keySize, const QString &outputDir, QString &logOutput);
};

#endif // MOKGENERATOR_H
