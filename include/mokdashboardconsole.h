#ifndef MOKDASHBOARDCONSOLE_H
#define MOKDASHBOARDCONSOLE_H

#include <QObject>
#include <QTextEdit>
#include "mokkeyentry.h"

class MokDashboardConsole : public QObject
{
    Q_OBJECT
public:
    explicit MokDashboardConsole(QTextEdit *displayWidget, QObject *parent = nullptr);

    // 🎨 Renders the colorful prototype status brief on boot
    void renderStartupBrief();

    // 🔬 Formats and prints a selected key's attributes with retro terminal coloring
    void updateWithKeyDetails(const MokKeyEntry &key);

private:
    QTextEdit *m_consoleDisplay;
};

#endif // MOKDASHBOARDCONSOLE_H
