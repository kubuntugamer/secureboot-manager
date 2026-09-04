#include "mokdashboardconsole.h"

MokDashboardConsole::MokDashboardConsole(QTextEdit *displayWidget, QObject *parent)
: QObject(parent), m_consoleDisplay(displayWidget)
{
    if (m_consoleDisplay) {
        // 📐 FONT SIZE UPGRADE: Boosted font-size to 13px for improved display clearance
        m_consoleDisplay->setStyleSheet(
            "background-color: #1e1e24;"
            "color: #a4b0be;"
            "font-family: 'Monospace', 'Courier New', monospace;"
            "font-size: 13px;"
            "border: 1px solid #3f4142;"
            "border-radius: 4px;"
            "padding: 12px;"
        );
    }
}

void MokDashboardConsole::renderStartupBrief()
{
    if (!m_consoleDisplay) return;

    // ⚡ EMBOLDENED RETRO-GLOW: Increased structural header bounds to 15px and text to 13px
    // 🛠️ TARGET ENVIRONMENT CLEANUP: Dropped noble reference to focus entirely on your custom target
    m_consoleDisplay->setHtml(
        "<div align='center' style='color: #00d2d3; font-weight: bold; font-size: 15px; margin-bottom: 6px;'>"
        "🔒 SECURE BOOT SUBSYSTEM INTERFACE"
        "</div>"
        "<hr style='border: 1px solid #34495e; margin-bottom: 12px;'>"
        "<div align='center' style='line-height: 1.5; font-size: 13px;'>"
        "<span style='color: #10ac84; font-weight: bold;'>⚡ [ STATUS: ONLINE & GUARDED ] ⚡</span><br>"
        "<span style='color: #54a0ff;'>Target Env:</span> <span style='color: #ff9f43;'>resolute</span><br><br>"
        "<span style='color: #8395a7;'>--- OPERATIONAL OVERVIEW ---</span><br>"
        "Select an active MOK entry row above<br>"
        "to stream raw <span style='color: #00d2d3;'>X.509 cryptographic</span><br>"
        "signature certificate dump data vectors<br>"
        "straight into this diagnostics console.<br>"
        "<span style='color: #8395a7;'>----------------------------</span>"
        "</div>"
    );
}

void MokDashboardConsole::updateWithKeyDetails(const MokKeyEntry &key)
{
    if (!m_consoleDisplay) return;

    // 📐 TRACKER ZOOM: Adjusted active row selections to match the new bold sizing
    QString htmlOutput = QString(
        "<div style='color: #ff9f43; font-weight: bold; font-size: 14px; margin-bottom: 4px;'>📋 X.509 Public Certificate Details</div>"
        "<hr style='border: 1px solid #34495e; margin-bottom: 8px;'>"
        "<div style='font-size: 13px; line-height: 1.4;'>"
        "<span style='color: #54a0ff;'>Common Name (CN):</span> <span style='color: #ffffff;'>%1</span><br>"
        "<span style='color: #54a0ff;'>Expiration:</span> <span style='color: #ffaa00;'>%2</span><br>"
        "<span style='color: #54a0ff;'>Serial Sequence:</span><br>"
        "<span style='color: #00d2d3;'>%3</span>"
        "</div>"
    ).arg(key.commonName, key.expirationDate, key.serialNumber);

    m_consoleDisplay->setHtml(htmlOutput);
}
