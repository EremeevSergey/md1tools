#include <QStringList>
#include <QtNumeric>
#include "extruder.h"

/*****************************************************************************\
*                                 CExtruderSet                                *
\*****************************************************************************/
CExtruder::CExtruder()
{
    Temperature = 0;
    On = false;
}

/*****************************************************************************\
*                                 CExtruderSet                                *
\*****************************************************************************/
CExtruderSet::CExtruderSet(CBasePrinterObject *parent) :
    CBasePrinterObject(parent)
{
    Name    = "Extruder";
}

CExtruderSet::~CExtruderSet()
{
    clear();
}

void CExtruderSet::setAmount (int amount)
{
    clear();
    for (int i=0;i<amount;i++)
        Items.append(CExtruder());
}

void CExtruderSet::clear()
{
    Items.clear();
}
/*
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
*/
