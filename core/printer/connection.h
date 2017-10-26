#ifndef CONNECTION_H
#define CONNECTION_H
#include <QObject>
#include <QSerialPort>
#include <QStringList>
#include <QByteArray>
#include <QTimerEvent>
#include "printer_object.h"

/*****************************************************************************\
*                               CConnection                                   *
*                       последовательный порт принтера                        *
\*****************************************************************************/
class CPrinter;
class CConnection:public QObject, public CBasePrinterObject
{
    Q_OBJECT
public:
    enum EDirection{
        Unknown =-1,
        Input   = 0,
        Output
    };
    enum EState{
        StateClosed  = 0,
        StateWaitStart,
        StateWaitWait,
        StateConnected
    };
    static char strInputFlag [];
    static char strOutputFlag[];
    static QStringList getSerialPortList();
public:
    CConnection(CPrinter *parent);
    ~CConnection();
    bool           open       ();
    bool           close      ();
    bool           isOpened   (){return State==StateConnected;}
    bool           isDataReady() {return InputBuffer.size()>1;}
    QString        readLine   ();
    bool           writeLine(const QByteArray& data);
    bool           writeLine(const char* data)   {return writeLine(QByteArray(data,strlen(data)));}
    bool           writeLine(const QString& data){return writeLine(data.toLocal8Bit());}

public:
    bool           setSerialPortBaudRate(qint32 br);
    inline qint32  getSerialPortBaudRate(){return SerialPortBaudRate;}
    bool           setSerialPortName    (const QString& name);
    inline QString getSerialPortName    (){return SerialPortName;}
protected:
    EState        State;
    void          clearInputBuffer();
private:
    QSerialPort*  SerialPort;
    qint32        SerialPortBaudRate;
    QStringList   InputBuffer;
    QString       SerialPortName;
protected slots:
    void          slotDataRecieved();
    void          handleError(QSerialPort::SerialPortError error);
signals:
    void          signalOpened   ();  // Сигнал "Соединение установлено"
    void          signalClosed   ();  // Сигнал "Соединение разорвано"
    void          signalDataReady();  // Сигнал "Входящие данные готовы"
    void          signalAddToLog (EDirection direction,
                                  const QString& string); // Сигнал Вывода входящих строк в консоль (лог)
};

/*class CConnection:public QObject
{
    Q_OBJECT
public:
    enum Direction{
        Unknown =-1,
        Input   = 0,
        Output
    };

    static char strInputFlag[];
    static char strOutputFlag[];
    static QStringList getSerialPortList();
    static QString     logStringToString(const QString& in,Direction* dir=0);
public:
    CConnection(QObject* parent=0);

    bool        setSerialPort(const QString& name);
    bool        setSerialPortBaudRate(qint32 br);
    bool        open();
    bool        close();
    bool        isDataReady();
    bool        isOpened(){return flConnected;}
    QString     readLine();
    bool        writeLine(const QByteArray& data);
    bool        writeLine(const char* data)   {return writeLine(QByteArray(data,strlen(data)));}
    bool        writeLine(const QString& data){return writeLine(data.toLocal8Bit());}
protected:
    void        clearInputBuffer();
    void        timerEvent(QTimerEvent *event);
protected:
    int         TimerId;
    QSerialPort SerialPort;
    bool        flConnected;
    bool        flStartConnection;
    QStringList InputBuffer;
signals:
    void        signalDataReady();
    void        signalOpened();
    void        signalClosed();
    void        signalAddToLog(const QString& str);
private slots:
    void        slotDataRecieved();
};
*/
#endif // CONNECTION_H
