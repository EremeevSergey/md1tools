#include <QtNumeric>
#include <math.h>
#include <QMenu>
#include "control_wnd.h"
#include "ui_control_wnd.h"
#include "../../common.h"
/*
- G90 - Use absolute coordinates
- G91 - Use relative coordinates
*/

CControlWindow::CControlWindow(QWidget *parent) :
    CBaseWindow(parent),
    ui(new Ui::control_wnd)
{
    cursorPosition.setX(qQNaN());
    cursorPosition.setY(qQNaN());
    ui->setupUi(this);
    connect(ui->rbHuman,SIGNAL(clicked()),SLOT(slotSwitchCoordinateSystem()));
    connect(ui->rbPrinter,SIGNAL(clicked()),SLOT(slotSwitchCoordinateSystem()));
    connect(ui->rbPolar,SIGNAL(clicked()),SLOT(slotSwitchCoordinateSystem()));

    setWindowTitle("NotReady Control.");
    setWindowIcon(QIcon(":/images/joystick.png"));
    BedWidget = new CBlackBedWidget();
//    BedWidget->setTransparency(128);
    BedWidget->setCursorVisible(true);
    BedWidget->setHotEndVisible(true);
    ui->bedLayout->addWidget(BedWidget);
    connect(BedWidget,SIGNAL(signalNewCursorPosition(QPointF)),SLOT(slotNewCursorPosition(QPointF)));
    connect(BedWidget,SIGNAL(signalRBMousePress(QPointF)),SLOT(slotRBMousePress(QPointF)));

    XControl = new CUpDownWidget("X");
    YControl = new CUpDownWidget("Y");
    ZControl = new CUpDownWidget("Z");

    ui->xyzLayout->addWidget(XControl,1);
    ui->xyzLayout->addWidget(YControl,1);
    ui->xyzLayout->addWidget(ZControl,1);

    connect(XControl,SIGNAL(signalUpDown(float)),SLOT(slotUpDownControls(float)));
    connect(YControl,SIGNAL(signalUpDown(float)),SLOT(slotUpDownControls(float)));
    connect(ZControl,SIGNAL(signalUpDown(float)),SLOT(slotUpDownControls(float)));

    updateExtruders();
    connect(&Printer,SIGNAL(signalNewPositionReady(TVertex)),SLOT(slotNewPositionReady(TVertex)));
    connect(&Printer,SIGNAL(signalOpened()),SLOT(slotOpened()));
    connect(&Printer,SIGNAL(signalReady(QString)),SLOT(slotReady(QString)));
    connect(&Printer,SIGNAL(signalBusy (QString)),SLOT(slotBusy (QString)));
    updateControls();


    ui->rbPolar->hide();
}

CControlWindow::~CControlWindow()
{
    clearExtruders();
    delete ui;
}

void CControlWindow::slotSwitchCoordinateSystem()
{
    updateControls();
}

void CControlWindow::updateControls()
{
    if (ui->rbPolar->isChecked()){
        ui->labelCoordX->setText(tr("Distance:"));
        ui->labelCoordY->setText(tr("Angle:"));
    }
    else{
        ui->labelCoordX->setText("X:");
        ui->labelCoordY->setText("Y:");
    }
    updateCursorPositionControls();
}

void CControlWindow::updateCursorPositionControls()
{
    QString sx,sy;
    QPointF point;
    if (ui->rbPolar->isChecked()){
        point = BedWidget->fromPrinterToPolar(cursorPosition);
        if (qIsNaN(point.x())) sx="--.----";
        else sx = QString::number(point.x(),'f',2);

        if (qIsNaN(point.y())) sy="--.----";
        else sy = QString::number(point.y()/M_PI*180.0,'f',2);
    }
    else{
        point = cursorPosition;
        if (ui->rbHuman->isChecked()) point = BedWidget->fromPrinterToHuman(cursorPosition);
        if (qIsNaN(point.x())) sx="--.----";
        else sx = QString::number(point.x(),'f',2);

        if (qIsNaN(point.y())) sy="--.----";
        else sy = QString::number(point.y(),'f',2);
    }
    ui->leCoordX->setText(sx);
    ui->leCoordY->setText(sy);
}

void CControlWindow::slotNewCursorPosition(const QPointF& pos)
{
    cursorPosition = pos;
    updateCursorPositionControls();
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
        extr->hidePosition(true);
        ExtrudersControl.append(extr);
        ui->extruderPositionLayout->addWidget(extr,1);
    }
}

void CControlWindow::slotNewPositionReady (const TVertex& ver)
{
    XControl->setPosition(ver.X);
    YControl->setPosition(ver.Y);
    ZControl->setPosition(ver.Z);
    BedWidget->setHotEndPosition(QPointF(ver.X,ver.Y));
}


void CControlWindow::on_pbHomeAll_clicked()
{
    Printer.sendGoHomeAll();
}

void CControlWindow::slotOpened ()
{
    updateExtruders();
    CEePromRecord* record = Printer.EEPROM->atByStartName("Max printable radius");
    if (record && !qIsNaN(record->FValue)){
        BedWidget->setBedRadius(record->FValue);
    }
}

void CControlWindow::slotReady (const QString&)
{
//    stop();
    ui->tabWidget->setEnabled(true);
    ui->frameLeft->setEnabled(true);
}

void CControlWindow::slotBusy  (const QString&)
{
//    start();
    ui->tabWidget->setEnabled(false);
    ui->frameLeft->setEnabled(false);
}

void CControlWindow::slotUpDownControls (float value)
{
    if      (sender()==XControl)  shiftXYZ(value,0,0);
    else if (sender()==YControl)  shiftXYZ(0,value,0);
    else if (sender()==ZControl)  shiftXYZ(0,0,value);
    else{

    }
}

void CControlWindow::on_pbGoXYZ_clicked()
{
    gotoXYZ(ui->dsbX->value(),ui->dsbY->value(),ui->dsbZ->value());
}

void CControlWindow::slotRBMousePress(const QPointF& pos)
{
    if (ui->tabWidget->currentIndex()==0){
        QMenu* menu = new QMenu(this);
        menu->addAction(tr("Go..."));
        tmpPoint = pos;
        connect(menu,SIGNAL(triggered(QAction*)),SLOT(slotTab0_goto(QAction*)));
        QPointF s = BedWidget->fromPrinterToScreen(pos);
        s= BedWidget->mapToGlobal(QPoint(s.x(),s.y()));
        menu->exec(QPoint(s.x(),s.y()));
    }
}

void CControlWindow::slotTab0_goto (QAction*)
{
    ui->dsbX->setValue(tmpPoint.x());
    ui->dsbY->setValue(tmpPoint.y());
    gotoXYZ(ui->dsbX->value(),ui->dsbY->value(),ui->dsbZ->value());
}


void CControlWindow::gotoXYZ(float x,float y,float z)
{
    CPrinterScript script;
    script.append(Printer.getGoToXYZString(x,y,z),true);
    script.append("M114",false);
    Printer.sendScript(script);
}

void CControlWindow::shiftXYZ(float x,float y,float z)
{
    CPrinterScript script;
    script.append("G91",false);
    script.append(Printer.getGoToXYZString(x,y,z),true);
    script.append("G90",false);
    script.append("M114",false);
    Printer.sendScript(script);
}

