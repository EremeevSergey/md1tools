#ifndef PRINTER_H
#define PRINTER_H
#include <QStringList>
#include "connection.h"
#include "eeprom.h"
#include "extruder.h"
#include "../types.h"

/*
Printer:
   Connection
   EEPROM
   Extruder
   Bed
*/

class CPrinter:public QObject, public CBasePrinterObject
{
    Q_OBJECT
public:
    static double getDoubleParameter(const QString &line, const QString& name);

    enum EState{
        PStateAbsent=0,
        PStateReady,
        PStateProcessCommand,
        PStateScriptPlaying,
        PStateReadEeprom,
        PStateReadInfo
    };

    CPrinter(QObject* parent=0);
    ~CPrinter();

public:
    CConnection*  Connection;
    CEeProm*      EEPROM;
//    CExtruderSet* Extruders;
    TVertex       CurrentPosition;
    TVertex       ZProbe;
public:
    bool        isReady            (){return State==PStateReady;}
    void        sendCmd            (const QString& cmd,bool wait);
    void        sendGoHomeAll      ();
    void        sendGoToXYZ        (double x,double y, double z);
    void        sendGetZProbeValue ();
    void        sendEmergencyReset ();
    void        sendScript         (const QStringList&    list);
    void        sendScript         (const CPrinterScript& list);
    void        setErrorString     (const QString& str,CBasePrinterObject* sender);
protected:
    EPrinterCommands CurrentCommandType;
    EState           State;
    bool             waitWait;
    CPrinterScript   Script;
protected:
    bool        parsePrinterAnswer(const QString& input,EPrinterCommands cmd_type);
    void        processOk         ();
    void        processWait       ();
    bool        __sendScriptLine   (EPrinterCommands cmd_type);
    bool        __sendCommand      (const QString&     cmd_string,
                                    EPrinterCommands   cmd_type,
                                    bool wait);
    bool        __sendCommands     (const CPrinterScript& cmd_string,
                                    EPrinterCommands cmd_type);
signals:
    void        signalBusy             (const QString&);
    void        signalReady            (const QString&);
    void        signalNewPositionReady (const TVertex& ver);
protected slots:
    void        slotOpened    ();
    void        slotClosed    ();
    void        slotDataReady ();




public:
//    void        clearLog       ();

protected:
//    QStringList LogBuffer;
    bool        processinCommand;
    int         currentScriptLine;

signals:
//    void        signalCommandExecuted ();
protected slots:
//    void        slotAddToLog  (const QString& str);
public:
};

#endif // PRINTER_H
