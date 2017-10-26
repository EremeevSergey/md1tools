#include "mainwindow.h"
#include <QApplication>
#include "common.h"
#include "test/plane_wnd.h"
#include "test/escher3d/escher3d_wnd.h"
#include "test/opendact/opendact_wnd.h"
#include "test/terminal.h"
#include "test/eeprom_conf.h"
#include "test/control/control_wnd.h"
#include "test/Delta-Bed-Calibration-Tool/dbct_wnd.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Config.load();
    Printer.Connection->setSerialPortName(Config.PortName);
    Printer.Connection->setSerialPortBaudRate(Config.BaudRate);

    MainWindow w;
    w.resize(Config.WindowsWidth,Config.WindowsHeight);

    w.addTask(new CEepromConfiguration     ());
    w.addTask(new CControlWindow           ());
    w.addTask(new CPlaneWindow             ());
    w.addTask(new CEscher3dWindow          ());
    w.addTask(new COpendactWnd             ());
//    w.addTask(new CDeltaBedCalibrationTools());
    w.addTask(new CTestTerminal            ());
    w.setActiveTask();

    w.show();

    return a.exec();
}

