#include "terminal.h"
#include "../common.h"

CTestTerminal::CTestTerminal(QWidget *parent) :
    CBaseWindow(parent),Ui::terminal()
{
    setupUi(this);
    setWindowTitle(tr("Console"));
    setWindowIcon(QIcon(":/images/terminal.png"));

    terminal = new CTerminalWidget();
    layout()->addWidget(terminal);

    connect(Printer.Connection,SIGNAL(signalAddToLog(CConnection::EDirection,QString)),
            SLOT(slotAddLine(CConnection::EDirection,QString)));
    connect(terminal,SIGNAL(signalNewText(QString)),SLOT(slotNewText(QString)));
}

CTestTerminal::~CTestTerminal()
{
}

void CTestTerminal::slotNewText(const QString& str)
{
    Printer.Connection->writeLine(str);
}

void CTestTerminal::slotAddLine (CConnection::EDirection direction,const QString& string)
{
    if (skipCheckBox->isChecked()){
        QString s = string.trimmed().toLower();
        if (s=="wait" || s.startsWith("busy")) return;
    }
    terminal->slotAddLine(direction,string);
}


void CTestTerminal::on_pbClear_clicked()
{
    terminal->clear();
}
