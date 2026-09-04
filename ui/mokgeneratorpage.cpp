#include "mokgeneratorpage.h"
#include "mokuigenerator.h"
#include <QVBoxLayout>

MokGeneratorPage::MokGeneratorPage(QWidget *parent) : QWidget(parent)
{
    // Call the original layout module to draw the UI components onto this clean container
    MokUiGenerator::setupGenerationPage(this,
                                        editGenCommonName,
                                        spinGenDays,
                                        btnGenerateMok,
                                        textGenerationLog);
}
