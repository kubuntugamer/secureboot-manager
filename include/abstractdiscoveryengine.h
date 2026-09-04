#ifndef ABSTRACTDISCOVERYENGINE_H
#define ABSTRACTDISCOVERYENGINE_H

#include <QObject>
#include <QStringList>

class AbstractDiscoveryEngine : public QObject
{
    Q_OBJECT
public:
    explicit AbstractDiscoveryEngine(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~AbstractDiscoveryEngine() = default;

    // Defines the single contract every backend plugin must fulfill
    virtual void discoverKernels() = 0;

signals:
    // Emitted when background processes complete successfully
    void discoveryFinished(const QStringList &unsignedNames,
                           const QStringList &unsignedPaths,
                           const QStringList &signedNames);
    void discoveryFailed(const QString& errorMessage);
};

#endif // ABSTRACTDISCOVERYENGINE_H
