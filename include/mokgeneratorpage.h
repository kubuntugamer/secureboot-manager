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

private:
    QLineEdit *editGenCommonName;
    QSpinBox *spinGenDays;
    QPushButton *btnGenerateMok;
    QTextBrowser *textGenerationLog;
};

#endif // MOKGENERATORPAGE_H
