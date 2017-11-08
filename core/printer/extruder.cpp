#include <QStringList>
#include <QtNumeric>
#include "extruder.h"
/*

m105 x0
ok 0
T:25.35 /0 B:21.53 /0 RAW1:3937 B@:0 @:0 RAW0:3909
M119
ok 0
endstops hit: x_max:L y_max:L z_max:L Z-probe state:L


M140 S<temp> F1 - Set bed target temp
m140 s50 f1
ok 0
TargetBed:50
m105
ok 0
T:25.70 /0 B:23.61 /50 B@:255 @:0
*/
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
