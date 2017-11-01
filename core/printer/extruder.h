#ifndef EXTRUDER_H
#define EXTRUDER_H
#include <QList>
#include <QString>
#include "printer_object.h"

/*****************************************************************************\
*                                 CExtruderSet                                *
\*****************************************************************************/
class CExtruder
{
public:
    CExtruder();
    float Temperature;
    bool  On;
};

/*****************************************************************************\
*                                 CExtruderSet                                *
\*****************************************************************************/

class CExtruderSet : public CBasePrinterObject
{
public:
    enum EState{
        Ready = 0,
        SetTemperature,
        GetTemperature,
        WriteOne
    };
public:
    explicit CExtruderSet(CBasePrinterObject *parent);
            ~CExtruderSet();
    int      count(){return Items.size();}
    void     setAmount(int amount);
//    bool     setTemperature (int extruderNumber, float temp, bool wait=false);
protected:
    QList<CExtruder> Items;
private:
    void          clear();
};

#include "printer.h"
#endif // EXTRUDER_H
