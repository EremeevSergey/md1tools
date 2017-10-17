#ifndef OPENDACT_H
#define OPENDACT_H

#include <QObject>
#include <QString>
#include <QList>
#include "opendact_user_var.h"
#include "opendact_eeprom.h"

class COpenDACT:public QObject
{
    Q_OBJECT
public:
    bool heightsSet;
    bool checkHeightsOnly;
    int  iterationNum;
    int  calibrationSelection;
    bool calibrationState;
    bool checkHeights;
    bool EEPROMReadOnly;
    bool calibrationComplete;
    bool calibrateInProgress;
    bool wasZProbeHeightSet;
    bool isHeuristicComplete;
private:
    int   position;
    float tempAccuracy;
public:
    COpenDACT(QObject* parent=0);
    ~COpenDACT();
    static float checkZero(float value);
    bool calibrate        ();
    bool fastCalibration();
    bool basicCalibration();
    void heuristicLearning();
private:
    bool checkAccuracy(float  X,float  XOpp,float  Y,float  YOpp,float  Z,float  ZOpp);
    void towerOffsets (float& X,float& XOpp,float& Y,float& YOpp,float& Z,float& ZOpp);
    void HRad         (float& X,float& XOpp,float& Y,float& YOpp,float& Z,float& ZOpp);
    void alphaRotation(float& X,float& XOpp,float& Y,float& YOpp,float& Z,float& ZOpp);
    void stepsPMM     (float& X,float& XOpp,float& Y,float& YOpp,float& Z,float& ZOpp);
signals:
    void signalLogConsole(const QString&);
    void signalLogConsole(const QString&,Qt::GlobalColor color);
    void signalSetAccuracyPoint(int iteration_num, float temp_accuracy);
};

extern CUserVariables  UserVariables;
extern COpendactEeprom EEPROM;
extern CHeights        Heights;
extern COpenDACT       OpenDACT;



#endif // OPENDACT_H
