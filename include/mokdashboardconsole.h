#ifndef MOKDASHBOARDCONSOLE_H
#define MOKDASHBOARDCONSOLE_H

#include <QObject>
#include "mokkeyentry.h"

class QTextEdit;

class MokDashboardConsole : public QObject
{
    Q_OBJECT
public:
    explicit MokDashboardConsole(QTextEdit *displayWidget, QObject *parent = nullptr);

    void renderStartupBrief();
    void renderGenerationBrief();
    void renderSigningBrief(); // ➕ Declare the missing blueprint name right here!
    void updateWithKeyDetails(const MokKeyEntry &key);

private:
    QTextEdit *m_consoleDisplay;
};

#endif // MOKDASHBOARDCONSOLE_H
