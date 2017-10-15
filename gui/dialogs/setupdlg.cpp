#include "setupdlg.h"
#include <QVBoxLayout>
//#include <QSpacerItem>

CSetupDialog::CSetupDialog(QWidget *parent) :
    QDialog(parent),Ui::setupdlg()
{
    setupUi(this);
    QWidget* tab1 = tabWidget->widget(0);
    serialSetup = new CSerialSetup(Qt::Vertical);

    QVBoxLayout* lay = new QVBoxLayout(tab1);
    lay->addWidget(serialSetup);
    lay->addStretch(1);
}

CSetupDialog::~CSetupDialog()
{
}

