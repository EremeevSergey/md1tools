#ifndef EEPROM_H
#define EEPROM_H
#include <QList>
#include <QString>

#include <QObject>

/*

EPR:_data_type position value description_
    The first value is the data_type (0=byte, 1=16 bit int,2=32 bit int,3 = float).
    The second parameter is the position in EEPROM, the value is stored.
    The third parameter is the current value.
    The rest of the line is a short description of the parameter.

M205 Repetier - Output EEPROM settings.
M205
ok 0
EPR:2 75 250000 Baudrate
EPR:0 1028 0 Language
EPR:3 129 15.335 Filament printed [m]
EPR:2 125 19263 Printer active [s]
EPR:2 79 0 Max. inactive time [ms,0=off]
EPR:2 83 360000 Stop stepper after inactivity [ms,0=off]
EPR:3 11 100.0000 Steps per mm
EPR:3 23 200.000 Max. feedrate [mm/s]
EPR:3 35 40.000 Homing feedrate [mm/s]
EPR:3 39 20.000 Max. jerk [mm/s]
EPR:3 133 0.000 X min pos [mm]
EPR:3 137 0.000 Y min pos [mm]
EPR:3 141 0.000 Z min pos [mm]
EPR:3 145 90.000 X max length [mm]
EPR:3 149 90.000 Y max length [mm]
EPR:3 153 297.060 Z max length [mm]
EPR:1 891 70 Segments/s for travel
EPR:1 889 180 Segments/s for printing
EPR:3 59 1000.000 Acceleration [mm/s^2]
EPR:3 71 2000.000 Travel acceleration [mm/s^2]
EPR:3 881 217.000 Diagonal rod length [mm]
EPR:3 885 94.080 Horizontal rod radius at 0,0 [mm]
EPR:3 925 90.000 Max printable radius [mm]
EPR:1 893 107 Tower X endstop offset [steps]
EPR:1 895 -16 Tower Y endstop offset [steps]
EPR:1 897 99 Tower Z endstop offset [steps]
EPR:3 901 209.950 Alpha A(210):
EPR:3 905 329.550 Alpha B(330):
EPR:3 909 90.000 Alpha C(90):
EPR:3 913 0.030 Delta Radius A(0):
EPR:3 917 0.400 Delta Radius B(0):
EPR:3 921 -0.430 Delta Radius C(0):
EPR:3 933 0.000 Corr. diagonal A [mm]
EPR:3 937 0.000 Corr. diagonal B [mm]
EPR:3 941 0.000 Corr. diagonal C [mm]
EPR:3 1024 0.000 Coating thickness [mm]
EPR:3 808 0.000 Z-probe height [mm]
EPR:3 929 5.000 Max. z-probe - bed dist. [mm]
EPR:3 812 5.000 Z-probe speed [mm/s]
EPR:3 840 40.000 Z-probe x-y-speed [mm/s]
EPR:3 800 0.000 Z-probe offset x [mm]
EPR:3 804 0.000 Z-probe offset y [mm]
EPR:3 816 0.000 Z-probe X1 [mm]
EPR:3 820 0.000 Z-probe Y1 [mm]
EPR:3 824 0.000 Z-probe X2 [mm]
EPR:3 828 0.000 Z-probe Y2 [mm]
EPR:3 832 4.300 Z-probe X3 [mm]
EPR:3 836 0.000 Z-probe Y3 [mm]
EPR:3 1036 0.000 Z-probe bending correction A [mm]
EPR:3 1040 0.000 Z-probe bending correction B [mm]
EPR:3 1044 0.000 Z-probe bending correction C [mm]
EPR:0 106 1 Bed Heat Manager [0-3]
EPR:0 107 255 Bed PID drive max
EPR:0 124 80 Bed PID drive min
EPR:3 108 196.000 Bed PID P-gain
EPR:3 112 33.020 Bed PID I-gain
EPR:3 116 290.000 Bed PID D-gain
EPR:0 120 255 Bed PID max value [0-255]
EPR:0 1020 0 Enable retraction conversion [0/1]
EPR:3 992 3.000 Retraction length [mm]
EPR:3 1000 40.000 Retraction speed [mm/s]
EPR:3 1004 0.000 Retraction z-lift [mm]
EPR:3 1008 0.000 Extra extrusion on undo retract [mm]
EPR:3 1016 20.000 Retraction undo speed
EPR:3 200 164.835 Extr.1 steps per mm
EPR:3 204 30.000 Extr.1 max. feedrate [mm/s]
EPR:3 208 10.000 Extr.1 start feedrate [mm/s]
EPR:3 212 4000.000 Extr.1 acceleration [mm/s^2]
EPR:0 216 1 Extr.1 heat manager [0-3]
EPR:0 217 140 Extr.1 PID drive max
EPR:0 245 60 Extr.1 PID drive min
EPR:3 218 24.0000 Extr.1 PID P-gain/dead-time
EPR:3 222 0.8800 Extr.1 PID I-gain
EPR:3 226 80.0000 Extr.1 PID D-gain
EPR:0 230 255 Extr.1 PID max value [0-255]
EPR:2 231 0 Extr.1 X-offset [steps]
EPR:2 235 0 Extr.1 Y-offset [steps]
EPR:2 290 0 Extr.1 Z-offset [steps]
EPR:1 239 1 Extr.1 temp. stabilize time [s]
EPR:1 250 150 Extr.1 temp. for retraction when heating [C]
EPR:1 252 0 Extr.1 distance to retract when heating [mm]
EPR:0 254 255 Extr.1 extruder cooler speed [0-255]
EPR:3 246 0.000 Extr.1 advance L [0=off]

*/


#include "printer_object.h"
/*****************************************************************************\
*                               CEePromRecord                                 *
*                         Класс одной записи EEPROM                           *
\*****************************************************************************/
class CEePromRecord
{
public:
    enum EType{
        Byte    = 0,
        Int16   = 1,
        Int32   = 2,
        Float   = 3,
        Unknown = -1
    };

public:
    int      Type;
    quint32  Position;
    float    FValue;
    qint32   IValue;
    QString  Description;
public:
    CEePromRecord();
    CEePromRecord(CEePromRecord* v);
    CEePromRecord(const CEePromRecord& v);

    bool     isEqual(const CEePromRecord* v) const;

    bool     FromString(const QString& str);
    QString  ToCmdString() {return ToCmdString(this);}
    static QString  ToCmdString(CEePromRecord* ptr);
    QString  ToString   () const;
    inline bool isValid() const;
    CEePromRecord& operator=(const CEePromRecord& right) {
            //проверка на самоприсваивание
            if (this == &right) return *this;
            Type        = right.Type;
            Position    = right.Position;
            FValue      = right.FValue;
            IValue      = right.IValue;
            Description = right.Description;
            return *this;
        }
};

/*****************************************************************************\
*                                CEePromList                                  *
*                        Класс списка значений EEPROM                         *
\*****************************************************************************/
class CEePromList
{
public:
    CEePromList ();
    CEePromList (const CEePromList &other);
    ~CEePromList();
    void           clear();
    int            count()const{return  ValueList.count();}
    CEePromRecord* at   (int i) const;
    CEePromRecord* atByStartName(const QString &name) const;
    void           append (const CEePromRecord* r);
    int            indexByName(const QString &name) const;
    int            indexByStartName(const QString &name) const;
    bool           setParameterValue(int index, qint32 val);
    bool           setParameterValue(int index,float   val);
protected:
    QList<CEePromRecord*> ValueList;
};

/*****************************************************************************\
*                                  CEeProm                                    *
*                                Класс EEPROM                                 *
\*****************************************************************************/
class CEeProm : public CBasePrinterObject,public CEePromList
{
public:
    CEeProm(CBasePrinterObject *parent);
    CEeProm(const CEePromList &other, CBasePrinterObject* parent);
    bool       read  ();
    bool       write ();
    bool       writeParameter    (int index);
    bool       writeParameter    (const QString& name);
    bool       parsePrinterAnswer(const QString& input,EPrinterCommands cmd_type);
protected:
//    int            ParameterIndex;
};

#endif // EEPROM_H
