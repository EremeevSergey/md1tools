#include "connection.h"
#include <QSerialPortInfo>
#include <QList>
#include <QDebug>

#define TIME_OUT_CONNECTION_MS 1000*60

char CConnection::strInputFlag []="IN:";
char CConnection::strOutputFlag[]="OUT:";


CConnection::CConnection(QObject *parent):QObject(parent)
{
    SerialPort.setBaudRate(115200);
    SerialPort.setDataBits(QSerialPort::Data8);
    SerialPort.setParity(QSerialPort::EvenParity);
    SerialPort.setFlowControl(QSerialPort::NoFlowControl);
    SerialPort.setStopBits(QSerialPort::TwoStop);
    connect(&SerialPort,SIGNAL(readyRead()),this,SLOT(slotDataRecieved()));
    clearInputBuffer();
    flConnected       = false;
    flStartConnection = false;
    TimerId     = 0;
}

void CConnection::clearInputBuffer()
{
    InputBuffer.clear();
    InputBuffer << "";
}

QStringList CConnection::getSerialPortList()
{
    QStringList ret;
    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    for (int i=0,n=list.size();i<n;i++){
        ret.append(list.at(i).portName());
    }
    return ret;
}

QString CConnection::logStringToString(const QString& in,Direction* dir)
{
    if (in.startsWith(strInputFlag)){
        if (dir) *dir = Input;
        return in.right(in.length()-strlen(strInputFlag));
    }
    else if (in.startsWith(strOutputFlag)){
        if (dir) *dir = Output;
        return in.right(in.length()-strlen(strOutputFlag));
    }
    else{
        if (dir) *dir = Unknown;
        return in;
    }

}

bool CConnection::open()
{
    if (!(flConnected | flStartConnection)){
        SerialPort.clearError();
        if (SerialPort.open(QIODevice::ReadWrite)){
            clearInputBuffer();
            flConnected       = false;
            flStartConnection = true;
            TimerId = startTimer(TIME_OUT_CONNECTION_MS);
            return true;
        }
        qDebug() << SerialPort.errorString();
    }
    return false;
}


bool CConnection::close()
{
    if (flConnected | flStartConnection){
        SerialPort.clearError();
        SerialPort.close();
        if (SerialPort.error()==QSerialPort::NoError){
            flConnected       = false;
            flStartConnection = false;
            if (TimerId) killTimer(TimerId);
            emit signalClosed();
            return true;
        }
    }
    return false;
}


bool CConnection::setSerialPort(const QString& name)
{
    if (!(flConnected | flStartConnection)){
        SerialPort.setPortName(name);
        return true;
    }
    return false;
}

bool CConnection::setSerialPortBaudRate(qint32 br)
{
    if (!(flConnected | flStartConnection))
        return SerialPort.setBaudRate(br);
    return false;
}

bool CConnection::isDataReady()
{
    return InputBuffer.size()>1;
//    int size=0;
//    for (int i=0,n=InputBuffer.size();i<n;i++)
//        size+= InputBuffer.at(i).size();
//    if (size>0) return true;
//    return false;
}

QString CConnection::readLine()
{
    QString ret;
    if (InputBuffer.size()>1){
        ret = InputBuffer.first();
        InputBuffer.removeFirst();
    }
//    if (InputBuffer.isEmpty())
//        InputBuffer<<"";
    return ret;
}

bool CConnection::writeLine(const QByteArray& data)
{
    if (flConnected){
        qint64 size=SerialPort.write(data+"\r\n");
        if (size>0)
            emit signalAddToLog(QString(strOutputFlag)+QString(data.left(size)));
        if (size==data.size()) return true;
    }
    return false;
}

void CConnection::slotDataRecieved()
{
    // Получили входные данные
    if (flStartConnection){
        flConnected = true;
        flStartConnection = false;
        emit signalOpened();
    }
    if (TimerId) killTimer(TimerId); // Сбрасываем таймер таймаута
    QByteArray in = SerialPort.readAll();
    int count=0;
    for (int i=0,n=in.size();i<n;i++){
        char ch = in.at(i);
        if (ch=='\n'){
//            if (InputBuffer.last()=="wait"){
//                count-=InputBuffer.last().size();
//                if (count<0) count=0;
//                InputBuffer.last().clear();
//            }
//            else{
                InputBuffer << "";
                count++;
//            }
        }
        else if (ch>=0x20){
            InputBuffer.last()+=QChar(ch);
            count++;
        }
    }
    if (count>0){
        for (int i=0,n=InputBuffer.size()-1;i<n;i++){
            emit signalAddToLog(QString(strInputFlag)+InputBuffer.at(i));
        }
        emit signalDataReady();
    }
    TimerId = startTimer(TIME_OUT_CONNECTION_MS);
}

void CConnection::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    close();
}

