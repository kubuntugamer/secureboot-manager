#include "mokremovalwizard.h"
#include <QStandardPaths>
#include <QFile>

// =========================================================================
// 🛡️ SYSTEM ENVIRONMENT VALIDATION CHAIN
// ASSOCIATED CODE FILES: include/mokremovalwizard.h, src/mokremovalwizard_verify.cpp
// ROADMAP STEP: Isolate low-level tool verification away from layout files
// =========================================================================
bool MokRemovalWizard::verifySystemToolchain()
{
    QStringList systemBinaries = {"sbverify", "sbattach", "mokutil", "pkexec"};

    // Hardcoded absolute system fallback directories where administrative utilities live natively
    QStringList structuralFallbacks = {"/usr/sbin", "/sbin", "/usr/bin", "/bin"};

    for (const QString &binary : systemBinaries) {
        // Phase 1: Attempt standard search lookup using the user's active env $PATH parameters
        QString locatedPath = QStandardPaths::findExecutable(binary);

        // Phase 2: If standard lookup returns empty, cross-check explicit secure system paths manually
        if (locatedPath.isEmpty()) {
            for (const QString &fallbackDir : structuralFallbacks) {
                QString testingTarget = fallbackDir + "/" + binary;
                if (QFile::exists(testingTarget)) {
                    locatedPath = testingTarget;
                    break; // Tool discovered successfully, escape fallback evaluation loop
                }
            }
        }

        // If both tracking vectors return empty, drop out—the runtime framework is missing tool requirements
        if (locatedPath.isEmpty()) {
            return false;
        }
    }

    return true;
}
