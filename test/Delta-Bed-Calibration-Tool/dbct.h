#ifndef DBCT_H
#define DBCT_H
#include <QList>
#include <QStringList>
#include "../../core/core.h"

class CDbctHeight
{
public:
    double X;
    double Y;
    double Z;
    double XOpp;
    double YOpp;
    double ZOpp;
public:
    CDbctHeight();
    QString toString();
};

class CPrinterConfig
{
public:
    double probeHeight;       // EPR:3 808 0.000 Z-probe height [mm]
    double stepPerMM;         // EPR:3 11 100.000 Steps per mm
    int    xEndstopOffset;    // EPR:1 893 107 Tower X endstop offset [steps]
    int    yEndstopOffset;    // EPR:1 895 -16 Tower Y endstop offset [steps]
    int    zEndstopOffset;    // EPR:1 897 99 Tower Z endstop offset [steps]
    double alphaA;            // EPR:3 901 209.950 Alpha A(210):
    double alphaB;            // EPR:3 905 329.550 Alpha B(330):
    double alphaC;            // EPR:3 909 90.000 Alpha C(90):
    double deltaRadiusA;      // EPR:3 913 0.030 Delta Radius A(0):
    double deltaRadiusB;      // EPR:3 917 0.400 Delta Radius B(0):
    double deltaRadiusC;      // EPR:3 921 -0.430 Delta Radius C(0):
    double diagonalRodLength; // EPR:3 881 217.000 Diagonal rod length [mm]
    double horizontalRodRadius;// EPR:3 885 94.080 Horizontal rod radius at 0,0 [mm]

public:
    CPrinterConfig();
    QString toString();
};

/*
class CDbctCalibrate
{
public:
    QStringList  console;
public:
    //Horizontal Radius Calibration*************************************************
    double A;// = parseFloat(document.getElementById('A').value);
    double B;// = parseFloat(document.getElementById('B').value);
    double C;// = parseFloat(document.getElementById('C').value);
    double DA;// = parseFloat(document.getElementById('DA').value);
    double DB;// = parseFloat(document.getElementById('DB').value);
    double DC;// = parseFloat(document.getElementById('DC').value);
    double HRad;// = parseFloat(document.getElementById('HRad').value);
    int    measInver;// = parseFloat(document.getElementById('measInver').value);
    double probeHeight;// = parseFloat(document.getElementById('probeHeight').value);

    double X;// = parseFloat(document.getElementById('X').value) * measInver - probeHeight;
    double Y;// = parseFloat(document.getElementById('Y').value) * measInver - probeHeight;
    double Z;// = parseFloat(document.getElementById('Z').value) * measInver - probeHeight;
    double XOpp;// = parseFloat(document.getElementById('XOpp').value) * measInver - probeHeight;
    double YOpp;// = parseFloat(document.getElementById('YOpp').value) * measInver - probeHeight;
    double ZOpp;// = parseFloat(document.getElementById('ZOpp').value) * measInver - probeHeight;

    double accuracy;// = parseFloat(document.getElementById('accuracy').value);

    //balance axes
    double offsetX;
    double offsetY;
    double offsetZ;
    double offsetXYZ;
    int    stepsPerMM;

    CDbctCalibrate();
    void   Calibrate(Ui::dbct_wnd* parent);
    double getPercentagesOpp(QChar _XYZ,double _X    ,double _Y    ,double _Z,
                                        double _XOpp ,double _YOpp ,double _ZOpp,
                                        double _X2   ,double _Y2   ,double _Z2,
                                        double _XOpp2,double _YOpp2,double _ZOpp2);
    double getPercentagesAll(QChar _XYZ,double _X    ,double _Y    ,double _Z,
                                        double _XOpp ,double _YOpp ,double _ZOpp,
                                        double _X2   ,double _Y2   ,double _Z2,
                                        double _XOpp2,double _YOpp2,double _ZOpp2);
    void calibVerify();
    double checkZero(double value);
};
*/

#endif // DBCT_H
