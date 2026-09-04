#ifndef MOKUIGENERATOR_H
#define MOKUIGENERATOR_H

class QWidget;
class QLineEdit;
class QSpinBox;
class QPushButton;
class QTextBrowser;

class MokUiGenerator {
public:
    // Single responsibility: Programmatically builds and styles all Page 2 input widgets cleanly
    static void setupGenerationPage(QWidget *pageContainer,
                                    QLineEdit *&commonNameEdit,
                                    QSpinBox *&validityDaysSpin,
                                    QPushButton *&generateBtn,
                                    QTextBrowser *&logBrowser);
};

#endif // MOKUIGENERATOR_H
