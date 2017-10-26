#include "control_wnd.h"
#include "ui_control_wnd.h"

CControlWindow::CControlWindow(QWidget *parent) :
    CBaseWindow(parent),
    ui(new Ui::control_wnd)
{
    ui->setupUi(this);
    setWindowTitle("Control.");
    setWindowIcon(QIcon(":/images/joystick.png"));
    BedWidget = new CBedWidgetBasic();
    ui->bedLayout->addWidget(BedWidget);
}

CControlWindow::~CControlWindow()
{
    delete ui;
}


