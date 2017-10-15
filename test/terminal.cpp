#include "terminal.h"

CTestTerminal::CTestTerminal(QWidget *parent) :
    CBaseWindow(parent),Ui::terminal()
{
    setupUi(this);
    setWindowTitle(tr("Терминал"));
    setWindowIcon(QIcon(":/images/terminal.png"));
}

CTestTerminal::~CTestTerminal()
{
}

void CTestTerminal::on_pushButton_clicked()
{
    QString str = cbCommand->currentText().trimmed();
    if (!str.isEmpty()){
        cbCommand->addItem(str);
        cbCommand->clearEditText();
        Printer.Connection->writeLine(str);
    }
}

