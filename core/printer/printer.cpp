#include "printer.h"
#include <QSerialPortInfo>
#include <QList>
#include <QDebug>
/*

Implemented Codes

- G0  -> G1
- G1  - Coordinated Movement X Y Z E, S1 disables boundary check, S0 enables it
- G4  - Dwell S<seconds> or P<milliseconds>
- G10 S<1 = long retract, 0 = short retract = default> retracts filament according to stored setting
- G11 S<1 = long retract, 0 = short retract = default> = Undo retraction according to stored setting
- G20 - Units for G0/G1 are inches.
- G21 - Units for G0/G1 are mm.
- G28 - Home all axis or named axis.
- G29 S<0..2> - Z-Probe at the 3 defined probe points. S = 1 measure avg. zHeight, S = 2 store avg zHeight
- G30 P<0..3> - Single z-probe at current position P = 1 first measurement, P = 2 Last measurement P = 0 or 3 first and last measurement
- G31 - Write signal of probe sensor
- G32 S<0..2> P<0..1> - Autolevel print bed. S = 1 measure zLength, S = 2 Measure and store new zLength
- G90 - Use absolute coordinates
- G91 - Use relative coordinates
- G92 - Set current position to coordinates given
- G131 - set extruder offset position to 0 - needed for calibration with G132
- G132 - calibrate endstop positions. Call this, after calling G131 and after centering the extruder holder.
- G133 - measure steps until max endstops for deltas. Can be used to detect lost steps within tolerances of endstops.
- G134 Px Sx Zx - Calibrate nozzle height difference (need z probe in nozzle!) Px = reference extruder, Sx = only measure extrude x against reference, Zx = add to measured z distance for Sx for correction.

RepRap M Codes

- M104 - Set extruder target temp
- M105 - Read current temp
- M106 S<speed> P<fan> - Fan on speed = 0..255, P = 0 or 1, 0 is default and can be omitted
- M107 P<fan> - Fan off, P = 0 or 1, 0 is default and can be omitted
- M109 - Wait for extruder current temp to reach target temp.
- M114 - Display current position

Custom M Codes

- M3 - Spindle on, Clockwise or Laser on during G1 moves.
- M4 - Spindle on, Counterclockwise.
- M5 - Spindle off, Laser off.
- M20  - List SD card
- M21  - Init SD card
- M22  - Release SD card
- M23  - Select SD file (M23 filename.g)
- M24  - Start/resume SD print
- M25  - Pause SD print
- M26  - Set SD position in bytes (M26 S12345)
- M27  - Report SD print status
- M28  - Start SD write (M28 filename.g)
- M29  - Stop SD write
- M30 <filename> - Delete file on sd card
- M32 <dirname> create subdirectory
- M42 P<pin number> S<value 0..255> - Change output of pin P to S. Does not work on most important pins.
- M80  - Turn on power supply
- M81  - Turn off power supply
- M82  - Set E codes absolute (default)
- M83  - Set E codes relative while in Absolute Coordinates (G90) mode
- M84  - Disable steppers until next move,
        or use S<seconds> to specify an inactivity timeout, after which the steppers will be disabled.  S0 to disable the timeout.
- M85  - Set inactivity shutdown timer with parameter S<seconds>. To disable set zero (default)
- M92  - Set axisStepsPerMM - same syntax as G92
- M99 S<delayInSec> X0 Y0 Z0 - Disable motors for S seconds (default 10) for given axis.
- M104 S<temp> T<extruder> P1 F1 - Set temperature without wait. P1 = wait for moves to finish, F1 = beep when temp. reached first time
- M105 X0 - Get temperatures. If X0 is added, the raw analog values are also written.
- M112 - Emergency kill
- M115- Capabilities string
- M116 - Wait for all temperatures in a +/- 1 degree range
- M117 <message> - Write message in status row on lcd
- M119 - Report endstop status
- M140 S<temp> F1 - Set bed target temp, F1 makes a beep when temperature is reached the first time
- M163 S<extruderNum> P<weight>  - Set weight for this mixing extruder drive
- M164 S<virtNum> P<0 = dont store eeprom,1 = store to eeprom> - Store weights as virtual extruder S
- M190 - Wait for bed current temp to reach target temp.
- M200 T<extruder> D<diameter> - Use volumetric extrusion. Set D0 or omit D to disable volumetric extr. Omit T for current extruder.
- M201 - Set max acceleration in units/s^2 for print moves (M201 X1000 Y1000)
- M202 - Set max acceleration in units/s^2 for travel moves (M202 X1000 Y1000)
- M203 - Set temperture monitor to Sx
- M204 - Set PID parameter X => Kp Y => Ki Z => Kd S<extruder> Default is current extruder. NUM_EXTRUDER=Heated bed
- M205 - Output EEPROM settings
- M206 - Set EEPROM value
- M207 X<XY jerk> Z<Z Jerk> E<ExtruderJerk> - Changes current jerk values, but do not store them in eeprom.
- M209 S<0/1> - Enable/disable autoretraction
- M220 S<Feedrate multiplier in percent> - Increase/decrease given feedrate
- M221 S<Extrusion flow multiplier in percent> - Increase/decrease given flow rate
- M226 P<pin> S<state 0/1> - Wait for pin getting state S. Add X0 to init as input without pullup and X1 for input with pullup.
- M231 S<OPS_MODE> X<Min_Distance> Y<Retract> Z<Backlash> F<ReatrctMove> - Set OPS parameter
- M232 - Read and reset max. advance values
- M233 X<AdvanceK> Y<AdvanceL> - Set temporary advance K-value to X and linear term advanceL to Y
- M251 Measure Z steps from homing stop (Delta printers). S0 - Reset, S1 - Print, S2 - Store to Z length (also EEPROM if enabled)
- M280 S<mode> - Set ditto printing mode. mode: 0 = off, 1 = 1 extra extruder, 2 = 2 extra extruder, 3 = 3 extra extruders
- M281 Test if watchdog is running and working. Use M281 X0 to disable watchdog on AVR boards. Sometimes needed for boards with old bootloaders to allow reflashing.
- M300 S<Frequency> P<DurationMillis> play frequency
- M302 S<0 or 1> - allow cold extrusion. Without S parameter it will allow. S1 will disallow.
- M303 P<extruder/bed> S<printTemerature> X0 R<Repetitions>- Autodetect pid values. Use P<NUM_EXTRUDER> for heated bed. X0 saves result in EEPROM. R is number of cycles.
- M320 S<0/1> - Activate autolevel, S1 stores it in eeprom
- M321 S<0/1> - Deactivate autolevel, S1 stores it in eeprom
- M322 - Reset autolevel matrix
- M323 S0/S1 enable disable distortion correction P0 = not permanent, P1 = permanent = default
- M340 P<servoId> S<pulseInUS> R<autoOffIn ms>: servoID = 0..3, Servos are controlled by a pulse with normally between 500 and 2500 with 1500ms in center position. 0 turns servo off. R allows automatic disabling after a while.
- M350 S<mstepsAll> X<mstepsX> Y<mstepsY> Z<mstepsZ> E<mstepsE0> P<mstespE1> : Set microstepping on RAMBO board
- M355 S<0/1> - Turn case light on/off, no S = report status
- M360 - show configuration
- M400 - Wait until move buffers empty.
- M401 - Store x, y and z position.
- M402 - Go to stored position. If X, Y or Z is specified, only these coordinates are used. F changes feedrate fo rthat move.
- M450 - Reports printer mode
- M451 - Set printer mode to FFF
- M452 - Set printer mode to laser
- M453 - Set printer mode to CNC
- M460 X<minTemp> Y<maxTemp> : Set temperature range for thermistor controlled fan
- M500 Store settings to EEPROM
- M501 Load settings from EEPROM
- M502 Reset settings to the one in configuration.h. Does not store values in EEPROM!
- M513 - Clear all jam marker.
- M600 Change filament
- M601 S<1/0> - Pause extruders. Paused extrudes disable heaters and motor. Unpausing reheats extruder to old temp.
- M602 S<1/0> P<1/0>- Debug jam control (S) Disable jam control (P). If enabled it will log signal changes and will not trigger jam errors!
- M908 P<address> S<value> : Set stepper current for digipot (RAMBO board)
- M999 - Continue from fatal error. M999 S1 will create a fatal error for testing.
*/
//char CPrinter::TaskName[] = "Printer";

CPrinter::CPrinter(QObject *parent):QObject(parent),CBasePrinterObject(0)
{
    Name = "Printer";
    Connection     = new CConnection(this);
    EEPROM         = new CEeProm(this);
    Extruders      = new CExtruderSet(this);

    CurrentCommandType = EPrinterCommandsNone; // Команда не обрабатывается
    State          = PStateAbsent;         // Принтер не доступен
    waitWait       = false;                // Не дожидаться wait
    connect(Connection,SIGNAL(signalOpened   ()),SLOT(slotOpened   ()),Qt::QueuedConnection);
    connect(Connection,SIGNAL(signalClosed   ()),SLOT(slotClosed   ()),Qt::QueuedConnection);
    connect(Connection,SIGNAL(signalDataReady()),SLOT(slotDataReady()),Qt::QueuedConnection);
}

CPrinter::~CPrinter()
{
    Connection->close();
//    delete EEPROM;
//    delete Connection;
}

bool CPrinter::__sendCommand(const QString& cmd_string,EPrinterCommands cmd_type,bool wait)
{
    bool ret=false;
    if (State==PStateReady){
        ret = Connection->writeLine(cmd_string);
        if (ret){
            CurrentCommandType=cmd_type;
            State=PStateProcessCommand;
            if (wait){
                waitWait = true;
//--                emit signalBusy(getName());
            }
        }

    }
    return ret;
}

bool CPrinter::__sendScriptLine(EPrinterCommands cmd_type)
{
    if (Script.size()>0){
        QString cmd_string = Script.first().Command;
        bool   wait = Script.first().Wait;
        Script.removeFirst();
        bool ret = Connection->writeLine(cmd_string);
        if (ret){
            CurrentCommandType=cmd_type;
//            if (wait)
//                waitWait = true;
            waitWait = wait;
            return true;
        }
    }
    return false;
}

bool CPrinter::__sendCommands(const CPrinterScript &cmd_string, EPrinterCommands cmd_type)
{
    if (State==PStateReady){
        Script = cmd_string;
        if (__sendScriptLine(cmd_type)){
            State=PStateScriptPlaying;
            emit signalBusy(getName());
            return true;
        }
    }
    return false;
}

void CPrinter::slotOpened    ()
{
    ZProbe             = TVertex();
    CurrentPosition    = TVertex();
    State              = PStateReady;
    CurrentCommandType = EPrinterCommandsNone;
    Capabilities.clear();
    waitWait           = false;
    __sendCommand("M115",EPrinterCommandsNone,true);
    State              = PStateReadInfo;
}

void CPrinter::slotClosed    ()
{
    State            = PStateAbsent;
    ZProbe           = TVertex();
    CurrentPosition  = TVertex();
    emit signalClosed();
}

bool CPrinter::parsePrinterAnswer(const QString& input,EPrinterCommands cmd_type)
{
    if (!CBasePrinterObject::parsePrinterAnswer(input,cmd_type)){
        // Если данные от Z-probe считываем их.
        QString input_low = input.toLower();
        if (input_low.startsWith("z-probe:")){
            ZProbe.X = getDoubleParameter(input_low,"x");
            ZProbe.Y = getDoubleParameter(input_low,"y");
            ZProbe.Z = getDoubleParameter(input_low,"z-probe");
            return true;
        }
        // Если данные текущем положении сопла считываем их.
        if (input_low.startsWith("x:") &&
            input_low.contains("y:") &&
            input_low.contains("z:")){
            CurrentPosition.X = getDoubleParameter(input_low,"x");
            CurrentPosition.Y = getDoubleParameter(input_low,"y");
            CurrentPosition.Z = getDoubleParameter(input_low,"z");
            emit signalNewPositionReady(CurrentPosition);
            return true;
        }
        // Здесь можно будет обработать например сообщения об ошибках
        //Unknown command:M900
        if (input_low.startsWith("unknown command:")){
            setErrorString (input,this);
            return true;
        }
        return false;
    }
    else return true;
}

void  CPrinter::processOk ()
{
    switch (State) {
    case PStateProcessCommand:
        if (waitWait!=true)
            State = PStateReady;
        break;
    case PStateScriptPlaying:
        if (waitWait!=true && !__sendScriptLine(CurrentCommandType)){
            State = PStateReady;
            emit signalReady(getName());
        }
        break;
    default:
        break;
    }
}

void CPrinter::processWait()
{
    switch (State) {
    case PStateReadInfo:
        // считали, считывам eeprom
        State = PStateReady;
        EEPROM->read();
        State = PStateReadEeprom;
        break;
    case PStateReadEeprom:
        // считали, идём домой
        State = PStateReady;
        sendGoHomeAll();
        State = PStateReadStartScript;
        break;
    case PStateReadStartScript:
        // Домой пришли, переходим в состояние готовности
        State = PStateReady;
        emit signalOpened();
        break;
    case PStateProcessCommand:
        State = PStateReady;
//--        emit signalReady(getName());
        break;
    case PStateScriptPlaying:
        if (!__sendScriptLine(CurrentCommandType)){
            State = PStateReady;
            emit signalReady(getName());
        }
        break;
    default:
        break;
    }
}

void  CPrinter::slotDataReady()
{
    while(Connection->isDataReady()){
        QString input = Connection->readLine().trimmed();
        QString str = input.toLower();
        if (str.startsWith("ok ")){
            processOk ();
        }
        else if (str=="wait"){
            if (CurrentCommandType!=EPrinterCommandsNone){
//                    qDebug() << getName() << CurrentCommandType;
                emit signalCommandReady (CurrentCommandType);
                CurrentCommandType=EPrinterCommandsNone;
            }
            processWait();
        }
        else if (str!="busy"){
            if (State==PStateReadInfo && Capabilities.count()<=0){
                Capabilities.parseString(input);
                __creatExtruders();
            }
            else parsePrinterAnswer(input,CurrentCommandType);
        }
    }
}

void CPrinter::__creatExtruders  ()
{
    bool ok;
    QString str = Capabilities.value("EXTRUDER_COUNT");
    if (str.size()>0){
        int count = str.toInt(&ok);
        if (ok){
            Extruders->setAmount(count);
        }
    }
}

void CPrinter::sendCmd (const QString& cmd, bool wait)
{
    if (State==PStateReady){
//        qDebug() << "+++++++++  cmd: " << cmd << " " << wait;
        __sendCommand(cmd,EPrinterCommandsNone,wait);
    }
}

void CPrinter::sendGoHomeAll ()
{
    if (State==PStateReady){
//        qDebug() << "+++++++++  cmdGoHomeAll";
//        Connection->writeLine("G28");
        __sendCommand("G28",EPrinterCommandsG28,true);
    }
}

void CPrinter::sendGoToXYZ        (double x,double y, double z)
{
    //   G1 X5.791 Y5.519 Z24
    if (State==PStateReady){
        CurrentPosition  = TVertex();
//        QString  str = QString("G1 X%1 Y%2 Z%3").arg(QString::number(x,'f',3),QString::number(y,'f',3),QString::number(z,'f',3));
//        qDebug() << "+++++++++  cmdGoToXYZ: " << str;
//        Connection->writeLine(str);
        __sendCommand(getGoToXYZString(x,y,z),EPrinterCommandsG1,true);
    }
}

QString CPrinter::getGoToXYZString (double x,double y, double z)
{
    return QString("G1 X%1 Y%2 Z%3").arg(QString::number(x,'f',3),QString::number(y,'f',3),QString::number(z,'f',3));
}


void CPrinter::sendGetZProbeValue ()
{
    if (State==PStateReady){
        ZProbe = TVertex();
//        Connection->writeLine("G30");// P2");
        __sendCommand("G30",EPrinterCommandsG30,true);
    }
}

void CPrinter::sendEmergencyReset()
{
    if (Connection->isOpened()){
        __sendCommand("M112",EPrinterCommandsM112,false);
    }
}

double CPrinter::getDoubleParameter(const QString& line,const QString& name)
{
    bool ok;
    QStringList list = line.split(' ');
    for (int i=0,n=list.size();i<n;i++){
        QStringList l1 = list.at(i).split(':');
        if (l1.size()==2 && l1.at(0)==name){
            double val = l1.at(1).toDouble(&ok);
            if (ok) return val;
        }
    }
    return qQNaN();
}

void CPrinter::setErrorString (const QString& str, CBasePrinterObject *sender)
{
    QString name;
    if (sender){
        name = getName();
        if (sender!=this)
            name = QString("%1.%2").arg(name,sender->getName());
    }
    qDebug() << name << ":" << str;
}
