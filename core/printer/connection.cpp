#include "connection.h"
#include <QSerialPortInfo>
#include <QList>
#include <QDebug>
#include <QMessageBox>

#define TIME_OUT_CONNECTION_MS 1000*60

#include "printer.h"

QStringList CConnection::getSerialPortList()
{
    QStringList ret;
    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    for (int i=0,n=list.size();i<n;i++)
        ret.append(list.at(i).portName());
    return ret;
}

CConnection::CConnection(CPrinter *parent):
    QObject(parent), CBasePrinterObject(parent)
{
    Name = "Connection";
    SerialPort = new QSerialPort();
    State = StateClosed;
    SerialPortBaudRate = 115200;
    clearInputBuffer();
//    connect(SerialPort, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
//            &MainWindow::handleError);

    connect(SerialPort,SIGNAL(error(QSerialPort::SerialPortError)),
//                static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error)),
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(SerialPort,SIGNAL(readyRead()),SLOT(slotDataRecieved()));

}

CConnection::~CConnection()
{
    delete SerialPort;
}

void CConnection::clearInputBuffer()
{
    InputBuffer.clear();
    InputBuffer << "";
}

bool CConnection::setSerialPortBaudRate(qint32 br)
{
    SerialPortBaudRate = br;
    if (State==StateConnected){
        if (!SerialPort->setBaudRate(SerialPortBaudRate,QSerialPort::AllDirections)){
            ParentObject->setErrorString(SerialPort->errorString(),this);
            return false;
        }
    return true;
    }
    return false;
}

bool CConnection::setSerialPortName (const QString& name)
{
    SerialPortName = name;
    SerialPort->setPortName(name);
    return true;
}

bool CConnection::open()
{
    if (State == StateClosed){
        SerialPort->clearError();
        if (SerialPort->open(QIODevice::ReadWrite) &&
            SerialPort->setBaudRate(SerialPortBaudRate,QSerialPort::AllDirections)){
            clearInputBuffer();
            State = StateWaitStart;
            //            TimerId = startTimer(TIME_OUT_CONNECTION_MS);
            InputBuffer.clear();
            return true;
        }
        else{
            if (ParentObject){
                ParentObject->setErrorString(SerialPort->errorString(),this);
            }
        }
    }
    return false;
}

bool CConnection::close()
{
    bool ret = false;
    if (State!=StateConnected){
        SerialPort->clearError();
        SerialPort->close();
        if (SerialPort->error()==QSerialPort::NoError){
            ret = true;
        }
        else{
            ParentObject->setErrorString(SerialPort->errorString(),this);
        }
        State = StateClosed;
        emit signalClosed();
    }
    return ret;
}

void CConnection::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(0,tr("Critical Error"), SerialPort->errorString());
        close();
    }
}

void CConnection::slotDataRecieved()
{
    if (State!=StateClosed){
        // Разбиваем полученные данные на строки
        QByteArray in = SerialPort->readAll();
        for (int i=0,n=in.size();i<n;i++){
            char ch = in.at(i);
            if (ch=='\n'){
                emit signalAddToLog(Input,InputBuffer.last());
                InputBuffer << "";
            }
            else if (ch>=0x20)
                InputBuffer.last()+=QChar(ch);
        }
        // Разбили на строки
        switch (State) {
        case StateWaitStart:
        case StateWaitWait:
            while (InputBuffer.size()>1){
                QString s = InputBuffer.first().toLower();
                InputBuffer.removeFirst();
                if (State == StateWaitStart){
                    if (s=="start") State = StateWaitWait;
                }
                else if (State == StateWaitWait){
                    if (s=="wait") {
                        State = StateConnected;
                        emit signalOpened ();
                        break;
                    }
                }
            }
            break;
        case StateConnected:
            if (InputBuffer.size()>1)  signalDataReady();
            break;
        default:
            break;
        }

    }
}

QString CConnection::readLine()
{
    QString ret;
    if (InputBuffer.size()>1){
        ret = InputBuffer.first();
        InputBuffer.removeFirst();
    }
    return ret;
}

bool CConnection::writeLine(const QByteArray& data)
{
    if (State!=StateClosed){
        qint64 size=SerialPort->write(data+"\r\n");
        if (size>0)
            emit signalAddToLog(Output,QString(data.left(size)));
        if (size==data.size()) return true;
    }
    return false;
}

