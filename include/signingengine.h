#ifndef SIGNINGENGINE_H
#define SIGNINGENGINE_H

#include <QObject>
#include <QProcess>

class SigningEngine : public QObject {
    Q_OBJECT
public:
    explicit SigningEngine(QObject *parent = nullptr);

    // Fires off sbsign and chains update-grub with full real-time feedback
    void injectSignature(const QString &binaryPath, const QString &keyPath, const QString &certPath);

    // Fires off sbattach to clean signatures out of a target binary image
    void stripSignature(const QString &binaryPath);

signals:
    // Emitted whenever a sub-process outputs text data to stream to the GUI terminal
    void logMessageReceived(const QString &text);

    // Emitted when the entire pipeline phase finishes processing completely
    void operationFinished(bool success, const QString &message);

private:
    void runBootloaderRefresh();
};

#endif // SIGNINGENGINE_H
