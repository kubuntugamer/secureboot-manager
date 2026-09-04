#include "mokdashboardconsole.h"
#include "mokconsoletemplates.h" // 📦 Include our isolated text string database
#include <QTextEdit>
#include <QTextCursor>

MokDashboardConsole::MokDashboardConsole(QTextEdit *displayWidget, QObject *parent)
: QObject(parent), m_consoleDisplay(displayWidget)
{
    if (m_consoleDisplay) {
        m_consoleDisplay->setStyleSheet(
            "background-color: #1e1e24;"
            "border: 1px solid #3f4142;"
            "border-radius: 4px;"
            "padding: 10px 14px 10px 14px;"
        );
        m_consoleDisplay->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_consoleDisplay->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_consoleDisplay->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    }
}

void MokDashboardConsole::renderStartupBrief()
{
    if (!m_consoleDisplay) return;
    m_consoleDisplay->setHtml(MokConsoleTemplates::StartupBrief);

    QTextCursor cursor = m_consoleDisplay->textCursor();
    cursor.movePosition(QTextCursor::Start);
    m_consoleDisplay->setTextCursor(cursor);
}

void MokDashboardConsole::renderGenerationBrief()
{
    if (!m_consoleDisplay) return;
    m_consoleDisplay->setHtml(MokConsoleTemplates::GenerationBrief);

    QTextCursor cursor = m_consoleDisplay->textCursor();
    cursor.movePosition(QTextCursor::Start);
    m_consoleDisplay->setTextCursor(cursor);
}

void MokDashboardConsole::renderSigningBrief()
{
    if (!m_consoleDisplay) return;
    m_consoleDisplay->setHtml(MokConsoleTemplates::SigningBrief);

    QTextCursor cursor = m_consoleDisplay->textCursor();
    cursor.movePosition(QTextCursor::Start);
    m_consoleDisplay->setTextCursor(cursor);
}

void MokDashboardConsole::updateWithKeyDetails(const MokKeyEntry &key)
{
    if (!m_consoleDisplay) return;

    // Inject the raw certificate dataset into the decoupled template container
    QString htmlOutput = QString(MokConsoleTemplates::KeyDetailsTemplate)
    .arg(key.commonName, key.expirationDate, key.serialNumber);
    m_consoleDisplay->setHtml(htmlOutput);

    QTextCursor cursor = m_consoleDisplay->textCursor();
    cursor.movePosition(QTextCursor::Start);
    m_consoleDisplay->setTextCursor(cursor);
}
