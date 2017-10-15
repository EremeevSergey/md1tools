#ifndef SERIAL_SETUP_H
#define SERIAL_SETUP_H
#include <QWidget>
#include <QComboBox>
#include <QStringList>
#include <QBoxLayout>


class CSerialSetup:public QWidget
{
    Q_OBJECT
public:
    CSerialSetup(Qt::Orientation orient,QWidget* parent=0);
    qint32  getBaudRate();
    QString getSerialName();
protected:
    QComboBox* portName;
    QComboBox* portBaudRate;
public slots:
    void    setSerialName(const QString& name);
    void    setBaudRate(qint32 br);
    void    updatePortList();
    void    updateBaudRateList();
private slots:
    void    slotPortChanged(const QString& name);
};

#endif // SERIAL_SETUP_H
