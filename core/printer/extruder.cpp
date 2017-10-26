#include <QStringList>
#include <QtNumeric>
#include "extruder.h"

/*****************************************************************************\
*                                 CExtruderSet                                *
\*****************************************************************************/
char CExtruderSet::TaskName[] = "Extruder.internal";


CExtruderSet::CExtruderSet(CPrinter *parent) :
    QObject(parent)
{
    Printer = parent;
    State   = Ready;
    CurrentItem = -1;

    connect(Printer->Connection,SIGNAL(signalAddToLog(QString)),SLOT(slotAnswer(QString)));
}

CExtruderSet::~CExtruderSet()
{
    clear();
}

bool CExtruderSet::setAmount (int amount)
{
    clear();
    for (int i=0;i<amount;i++)
        Items.append(qQNaN());
}

void CExtruderSet::clear()
{
//    for (int i=0,n= Items.size();i<n;i++){
//        delete Items.at(i);
//    }
    Items.clear();
}

bool CExtruderSet::setTemperature (int extruderNumber,float temp, bool wait)
{
//    - M104 S<temp> T<extruder> P1 F1 - Set temperature without wait.
//                P1 = wait for moves to finish
//                F1 = beep when temp. reached first time

    if (extruderNumber>=0 && extruderNumber<Items.size() &&
        State==Ready && Printer->Connection->isOpened()){
        QString line = QString("M104 S%1 T%2").
                               arg(QString::number(int(temp+0.5)),
                                   QString::number(extruderNumber));
        if (wait) {
            line+=" P1";
            CurrentItem = extruderNumber;
            emit signalBusy (TaskName);
            State = SetTemperature;
        }
        Printer->Connection->writeLine(line);
        return true;
    }
    return false;
}

bool CExtruderSet::updateData(int extruderNumber)
{
    return false;
}

void CExtruderSet::slotAnswer(const QString& str)
{
/*    if (State != Ready){
        CConnection::Direction dir;
        QString s = CConnection::logStringToString(str,&dir);
        if (dir == CConnection::Input){
            if (s.trimmed().toLower() == "wait"){
                // Command Executed
                Busy = Ready;
                emit signalReady (TaskName);
            }
            switch (Busy) {
            case ReadAll:
                r = new CEePromRecord();
                if (r->FromString(s)) ValueList.append(r);
                else delete r;
                break;
            default:
                break;
            }
        }
    }
    */
}
