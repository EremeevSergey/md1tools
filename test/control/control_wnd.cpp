#include "control_wnd.h"
#include "ui_control_wnd.h"
#include "../../common.h"

CControlWindow::CControlWindow(QWidget *parent) :
    CBaseWindow(parent),
    ui(new Ui::control_wnd)
{
    ui->setupUi(this);
    setWindowTitle("NotReady Control.");
    setWindowIcon(QIcon(":/images/joystick.png"));
    BedWidget = new CBlackBedWidget();
    BedWidget->setTransparency(128);
    ui->bedLayout->addWidget(BedWidget);

    XControl = new CUpDownWidget("X");
    YControl = new CUpDownWidget("Y");
    ZControl = new CUpDownWidget("Z");

    ui->xyzLayout->addWidget(XControl,1);
    ui->xyzLayout->addWidget(YControl,1);
    ui->xyzLayout->addWidget(ZControl,1);

    updateExtruders();
    connect(&Printer,SIGNAL(signalNewPositionReady(TVertex)),SLOT(slotNewPositionReady(TVertex)));
    connect(&Printer,SIGNAL(signalOpened()),SLOT(slotOpened()));
}

CControlWindow::~CControlWindow()
{
    clearExtruders();
    delete ui;
}

void CControlWindow::clearExtruders()
{
    for (int i=0,n=ExtrudersControl.size();i<n;i++){
        delete ExtrudersControl.at(i);
    }
    ExtrudersControl.clear();
}

void CControlWindow::updateExtruders()
{
    clearExtruders();
    int n = Printer.Extruders->count();
    ui->extruderPositionFrame->setVisible(n>0);
    for (int i=0;i<n;i++){
        CUpDownWidget* extr = new CUpDownWidget(QString("Extruder %1").arg(QString::number(i)));
        ExtrudersControl.append(extr);
        ui->extruderPositionLayout->addWidget(extr,1);
    }
}

void CControlWindow::slotNewPositionReady (const TVertex& ver)
{
    XControl->setPosition(ver.X);
    YControl->setPosition(ver.Y);
    ZControl->setPosition(ver.Z);
}


void CControlWindow::on_pbHomeAll_clicked()
{
    Printer.sendGoHomeAll();
}

void CControlWindow::slotOpened ()
{
    updateExtruders();
}

void CControlWindow::on_pbGoXYZ_clicked()
{

    Printer.sendGoToXYZ(ui->dsbX->value(),ui->dsbY->value(),ui->dsbZ->value());
}
