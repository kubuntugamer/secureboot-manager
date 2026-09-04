#ifndef DEBIANDISCOVERYENGINE_H
#define DEBIANDISCOVERYENGINE_H

#include "abstractdiscoveryengine.h"
#include <QStringList>

class DebianDiscoveryEngine : public AbstractDiscoveryEngine
{
    Q_OBJECT
public:
    explicit DebianDiscoveryEngine(QObject *parent = nullptr);
    ~DebianDiscoveryEngine() override = default;

    // Triggers the asynchronous dpkg-query sequence
    void discoverKernels() override;

private:
    // Sequentially steps through package listings to avoid thread collision
    // 🛠️ UPDATED FOOTPRINT: Now references the pre-flight signature cache to stop spam prompts
    void processNextPackage(const QStringList &packages, int index,
                            QStringList uNames, QStringList uPaths, QStringList sNames,
                            const QStringList &signedCachePaths);
};

#endif // DEBIANDISCOVERYENGINE_H
