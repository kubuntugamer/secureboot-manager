#ifndef MOKUIGENERATOR_H
#define MOKUIGENERATOR_H

class QWidget;
class QLineEdit;
class QSpinBox;
class QPushButton;
class QTextBrowser;

class MokUiGenerator {
public:
    // 🔗 POINTER REFERENCE SIGNATURE: Passing parameters by reference (*&) allows assigning components back to the parent class
    static void setupGenerationPage(QWidget *parent,
                                    QLineEdit *&editCommonName,
                                    QSpinBox *&spinDays,
                                    QPushButton *&btnGenerate,
                                    QTextBrowser *&textLog);
};

#endif // MOKUIGENERATOR_H
