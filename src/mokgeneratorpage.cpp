#include "mokgeneratorpage.h"
#include "mokuigenerator.h"
#include "mokgenerator.h"
#include "mokenrollmentwizard.h"

MokGeneratorPage::MokGeneratorPage(QWidget *parent)
: QWidget(parent)
{
    MokUiGenerator::setupGenerationPage(this,
                                        editGenCommonName,
                                        spinGenDays,
                                        btnGenerateMok,
                                        textGenerationLog);
}

void MokGeneratorPage::executeMokKeyPairGeneration()
{
    // Implementation body satisfies slot declared in header
}
