#ifndef DBCT_WND_H
#define DBCT_WND_H

#include "../../gui/basewnd.h"
#include "../../gui/basewnd.h"
#include "ui_dbct_wnd.h"
#include "dbct.h"

class CDeltaBedCalibrationTools : public CBaseWindow,public Ui::dbct_wnd
{
    Q_OBJECT
public:
    explicit CDeltaBedCalibrationTools(QWidget *parent = 0);
    ~CDeltaBedCalibrationTools();

protected:
    CPrinterConfig PrinterConfig;
protected:
    int  TestPlaneRadius;
    int  TestProbeStartOffset;
    int  step;
    int  state;
    int  currentVertexIndex;
    int  currentHeightIndex;
protected:
    bool gotoxyz       ();
//    void displayStat   ();
    void readEeprom    ();
    void updateControls();
    void mainLoop      ();
    void activeLoop    ();
    QStringList PrepareScript();

private slots:
    void slotCommandExecuted();
    void slotReady(const QString&);
private slots:
    void on_pbCalibrate_clicked();
    void on_pbStop_clicked();
    void on_pbStart_clicked();

protected:
// Delta Calibration
    //Horizontal Radius Calibration*************************************************
    double A;// = parseFloat(document.getElementById('A').value);
    double B;// = parseFloat(document.getElementById('B').value);
    double C;// = parseFloat(document.getElementById('C').value);
    double DA;// = parseFloat(document.getElementById('DA').value);
    double DB;// = parseFloat(document.getElementById('DB').value);
    double DC;// = parseFloat(document.getElementById('DC').value);
    double HRad;// = parseFloat(document.getElementById('HRad').value);
    double measInver;// = parseFloat(document.getElementById('measInver').value);
    double probeHeight;// = parseFloat(document.getElementById('probeHeight').value);

    CDbctHeight    Height[6];

    double accuracy;// = parseFloat(document.getElementById('accuracy').value);

    //balance axes
    double offsetX;
    double offsetY;
    double offsetZ;
    double offsetXYZ;
    double stepsPerMM;

    double X;
    double Y;
    double Z;
    double XOpp;
    double YOpp;
    double ZOpp;

    double X1;
    double Y1;
    double Z1;
    double XOpp1;
    double YOpp1;
    double ZOpp1;

    double X2;
    double Y2;
    double Z2;
    double XOpp2;
    double YOpp2;
    double ZOpp2;

    double X3;
    double Y3;
    double Z3;
    double XOpp3;
    double YOpp3;
    double ZOpp3;

    double X4;
    double Y4;
    double Z4;
    double XOpp4;
    double YOpp4;
    double ZOpp4;

    double X5;
    double Y5;
    double Z5;
    double XOpp5;
    double YOpp5;
    double ZOpp5;

    double HRadSA;

protected:
    void   calculate ();
    double getPercentagesOpp(char _XYZ,double _X    ,double _Y    ,double _Z,
                                        double _XOpp ,double _YOpp ,double _ZOpp,
                                        double _X2   ,double _Y2   ,double _Z2,
                                        double _XOpp2,double _YOpp2,double _ZOpp2);
    double getPercentagesAll(char _XYZ,double _X    ,double _Y    ,double _Z,
                                        double _XOpp ,double _YOpp ,double _ZOpp,
                                        double _X2   ,double _Y2   ,double _Z2,
                                        double _XOpp2,double _YOpp2,double _ZOpp2);
    void calibVerify();
    double checkZero(double value);
};

#endif // DBCT_WND_H
