#ifndef MOKGENERATORPAGE_H
#define MOKGENERATORPAGE_H

#include <QWidget>

class QLineEdit;
class QSpinBox;
class QPushButton;
class QTextBrowser;

class MokGeneratorPage : public QWidget
{
    Q_OBJECT

public:
    explicit MokGeneratorPage(QWidget *parent = nullptr);

public slots:
    void executeMokKeyPairGeneration();

private:
    QLineEdit *editGenCommonName = nullptr;
    QSpinBox *spinGenDays = nullptr;
    QPushButton *btnGenerateMok = nullptr;
    QTextBrowser *textGenerationLog = nullptr;
};

#endif // MOKGENERATORPAGE_H
