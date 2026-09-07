// ============================================================================
// HEADER SEPARATION FRAGMENT 1 OF 2: INCLUSIONS & CORE PROFILE STRUCTURES
// ============================================================================

#ifndef MOKSIGNERPAGE_H
#define MOKSIGNERPAGE_H

#include <QWidget>
#include <QString>
#include <QList>

class QLineEdit;
class QComboBox;
class QPushButton;
class AbstractDiscoveryEngine;

// 💎 KEY MANAGEMENT DATA MODEL: Holds isolated paths for cryptographic key pairs
struct KeyProfile {
    QString displayName;
    QString privateKeyPath;
    QString certificatePath;
    QString originPath;
};

class MokSignerPage : public QWidget
{
    Q_OBJECT
public:
    explicit MokSignerPage(QWidget *parent = nullptr);
    // ============================================================================
    // HEADER SEPARATION FRAGMENT 2 OF 2: PRIVATE MEMBER ATTRIBUTES & METHODS
    // ============================================================================

private:
    QLineEdit *editSignTargetPath = nullptr;
    QComboBox *m_keyComboBox = nullptr; // Replacing the legacy editSignKeyPath LineEdit
    QPushButton *btnBrowseBinary = nullptr;
    QPushButton *btnExecuteSignature = nullptr;

    AbstractDiscoveryEngine *discoveryEngine = nullptr;

    // 🛠️ INTERNAL REFACTOR METHODS: Drives drop-down mapping and metadata parsing
    void populateKeySelector();
    QString findMatchingCertificate(const QString &dirPath, const QString &baseName);

    // ✨ CERTIFICATE PROCESSING ENGINE: Reads CN attributes from the public verification asset
    QString extractCommonName(const QString &certPath);

    void setupExecutionHook();
    void triggerScan();

    // 💾 PROFILE CACHE VECTOR: Tracks currently loaded key structures
    QList<KeyProfile> m_keyProfiles;
};

#endif // MOKSIGNERPAGE_H
