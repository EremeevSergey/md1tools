#ifndef PRINTER_H
#define PRINTER_H
#include <QStringList>
#include "connection.h"
#include "eeprom.h"
#include "../types.h"

class CPrinter:public QObject
{
    Q_OBJECT
public:
    static   char TaskName[];

    enum EState{
        Ready=0,
        WaitStart,
        WaitWait,
        ScriptPlaying
    };

    CPrinter(QObject* parent=0);
    ~CPrinter();

public:
    CConnection* Connection;
    CEeProm*     EEPROM;
    TVertex      CurrentPosition;
    TVertex      ZProbe;
public:
    void        clearLog       ();
public:
    void        sendCmd           (const QString& cmd,bool wait);
    void        cmdGoHomeAll      ();
    void        cmdGoToXYZ        (double x,double y, double z);
    void        cmdGetZProbeValue ();
    void        emergencyReset();
    void        playScript (const QStringList& list);
protected:
    QStringList LogBuffer;
    bool        processinCommand;
    EState      State;
    QStringList Script;
    int         currentScriptLine;

signals:
    void        signalCommandExecuted ();
    void        signalBusy       (const QString&);
    void        signalReady      (const QString&);
    void        signalNewPosition(const TVertex& ver);
protected slots:
    void        slotOpened    ();
    void        slotClosed    ();
    void        slotAddToLog  (const QString& str);
    void        slotDataReady ();
public:
    static double getDoubleParameter(const QString &line, const QString& name);
};

#endif // PRINTER_H
