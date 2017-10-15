#ifndef CONNECTION_H
#define CONNECTION_H
#include <QObject>
#include <QSerialPort>
#include <QStringList>
#include <QByteArray>
#include <QTimerEvent>


class CConnection:public QObject
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

#endif // CONNECTION_H
