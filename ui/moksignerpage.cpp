#include "moksignerpage.h"
#include "mokuisigner.h"
#include <QVBoxLayout>

MokSignerPage::MokSignerPage(QWidget *parent) : QWidget(parent)
{
    // Delegate to your existing layout engine to paint the UI controls onto this container
    MokUiSigner::setupSigningPage(this,
                                  editSignTargetPath,
                                  editSignKeyPath,
                                  btnBrowseBinary,
                                  btnExecuteSignature);
}
