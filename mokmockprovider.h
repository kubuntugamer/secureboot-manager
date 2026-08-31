#ifndef MOKMOCKPROVIDER_H
#define MOKMOCKPROVIDER_H

#include <QVector>
#include "mokkeyentry.h"

class MokMockProvider {
public:
    static QVector<MokKeyEntry> getMockKeys();
};

#endif // MOKMOCKPROVIDER_H
