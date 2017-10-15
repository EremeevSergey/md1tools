#ifndef BASEWND_H
#define BASEWND_H

#include <QWidget>

class CBaseWindow : public QWidget
{
    Q_OBJECT
public:
    enum EState{
        Stoped = 0,
        Active,
        WaitPaused,
        Paused,
        WaitStop
    };

    enum EStep{
        NoStep = 0,
        GoHome ,
        GoToXYZ,
        ZProbe,
        ExecuteCommand,
        Process,
        ReadEEPROM,
        CheckHieghts,
        CheckZProbe,
        Calibrate,
        ReadEEPROMOnly
    };

    explicit CBaseWindow(QWidget *parent = 0);
    virtual void    reset();
protected:
    int             step;
    int             state;
    virtual void    start();
    virtual void    stop ();
//    virtual void    scriptReady();
signals:
    void    signalStart(const QString& name);
    void    signalStop (const QString& name);
public slots:
//    void    slotScriptReady ();
};

#endif // BASEWND_H
