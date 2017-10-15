#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSerialPortInfo>
#include "serial_setup.h"

CSerialSetup::CSerialSetup(Qt::Orientation orient,QWidget *parent):QWidget(parent)
{
    QHBoxLayout* lay1 = new QHBoxLayout();
    QLabel*      lb   = new QLabel(this);
    lb->setText(tr("Port:"));
    portName = new QComboBox(this);
    portName ->setEditable(false);
    lay1->addWidget(lb);
    lay1->addWidget(portName);

    QHBoxLayout* lay2 = new QHBoxLayout();
    lb   = new QLabel(this);
    lb->setText(tr("BaudRate:"));
    portBaudRate = new QComboBox(this);
    portBaudRate ->setEditable(false);
    lay2->addWidget(lb);
    lay2->addWidget(portBaudRate);

    QBoxLayout* lay = new QBoxLayout(QBoxLayout::TopToBottom,this);
    if (orient == Qt::Horizontal) lay->setDirection(QBoxLayout::LeftToRight);
    lay->addLayout(lay1);
    lay->addLayout(lay2);
    this->setLayout(lay);
    portBaudRate->setCurrentText(QString::number(115200));
    updatePortList();
    updateBaudRateList();
    portBaudRate->setCurrentText(QString::number(115200));
    connect (portName,SIGNAL(currentIndexChanged(QString)),this,SLOT(slotPortChanged(QString)));
}

qint32 CSerialSetup::getBaudRate()
{
    return portBaudRate->currentText().toULong();
}

QString CSerialSetup::getSerialName()
{
//    QString ret;
    return portName->currentText();
}

void CSerialSetup::setSerialName(const QString& name)
{
    portName->setCurrentText(name);
}

void CSerialSetup::setBaudRate(qint32 br)
{
    portBaudRate->setCurrentText(QString::number(br));
}

void CSerialSetup::updatePortList()
{
    QString current = portName->currentText();
    portName->clear();
    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    for (int i=0,n=list.size();i<n;i++){
        portName->addItem(list.at(i).portName());
    }
    portName->setCurrentText(current);
    updateBaudRateList();
}

void CSerialSetup::updateBaudRateList()
{
    QString current = portBaudRate->currentText();
    portBaudRate->clear();
    QString port = portName->currentText();
    if (!port.isEmpty()){
        QSerialPortInfo pi = QSerialPortInfo(port);
        QList<qint32> list = pi.standardBaudRates();
        for (int i=0,n=list.size();i<n;i++){
            portBaudRate->addItem(QString::number(list.at(i)));
        }
        portBaudRate->setCurrentText(current);
    }
}

void CSerialSetup::slotPortChanged(const QString& name)
{
    Q_UNUSED(name)
    updateBaudRateList();
}
