#ifndef SETUPDLG_H
#define SETUPDLG_H

#include <QDialog>
#include "ui_setupdlg.h"
#include "gui/widgets/serial_setup.h"

class CSetupDialog : public QDialog,public Ui::setupdlg
{
    Q_OBJECT

public:
    explicit CSetupDialog(QWidget *parent = 0);
    ~CSetupDialog();

    void    setSerialPortName(const QString& name) {serialSetup->setSerialName(name);}
    void    setSerialBaudRate(qint32 brate)        {serialSetup->setBaudRate  (brate);}
    QString getSerialPortName(){return serialSetup->getSerialName();}
    qint32  getSerialBaudRate(){return serialSetup->getBaudRate();}

protected:
    CSerialSetup* serialSetup;

};

#endif // SETUPDLG_H
