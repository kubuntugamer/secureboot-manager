#ifndef MOKPROVIDER_H
#define MOKPROVIDER_H

#include <QVector>
#include "mokkeyentry.h"

class MokProvider {
public:
    // Single responsibility: Direct system integration loop harvesting real NVRAM MOK arrays via mokutil
    static QVector<MokKeyEntry> getLiveKeys();
};

#endif // MOKPROVIDER_H
