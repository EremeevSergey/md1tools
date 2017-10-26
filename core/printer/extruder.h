#ifndef EXTRUDER_H
#define EXTRUDER_H
#include <QList>
#include <QString>

#include <QObject>
/*****************************************************************************\
*                                 CExtruderSet                                *
\*****************************************************************************/
class CPrinter;

class CExtruderSet : public QObject
{
    Q_OBJECT
public:
    enum EState{
        Ready = 0,
        SetTemperature,
        GetTemperature,
        WriteOne
    };
public:
    static   char TaskName[];

    explicit CExtruderSet(CPrinter *parent);
            ~CExtruderSet();
    int      count(){return Items.size();}
    bool     setAmount      (int amount);
    bool     updateData     (int extruderNumber);
    bool     setTemperature (int extruderNumber, float temp, bool wait=false);
protected:
    CPrinter*     Printer;
    QList<float>  Items;
    EState        State;
    int           CurrentItem;
private:
    void          clear();
signals:
    void          signalBusy (const QString&);
    void          signalReady(const QString&);
public slots:
    void          slotAnswer (const QString& str);
};

#include "printer.h"
#endif // EXTRUDER_H
