#include "opendact.h"
#include <math.h>
#include <QtNumeric>
#include "common.h"
#include <QDebug>

CUserVariables  UserVariables;
COpendactEeprom EEPROM;
CHeights        Heights;
COpenDACT       OpenDACT;

COpenDACT::COpenDACT(QObject *parent):QObject(parent)
{
    heightsSet       = false;
    checkHeightsOnly = false;
    position         = 0;
    calibrationState = false;
    checkHeights     = false;
    EEPROMReadOnly   = false;
    calibrationComplete = false;
    calibrateInProgress = false;
    wasZProbeHeightSet  = false;
    isHeuristicComplete = false;
    calibrationSelection = 0;
}

COpenDACT::~COpenDACT()
{

}

float COpenDACT::checkZero(float value)
{
    if (value > -0.001 && value < 0.001) return 0;
    else                                 return value;
}

bool COpenDACT::calibrate()
{
    bool ret=true;
    if      (calibrationSelection == 0) ret=basicCalibration();
    else if (calibrationSelection == 1) ret=fastCalibration();
    iterationNum++;
    return ret;
}

bool COpenDACT::fastCalibration()
{
    //check if eeprom object remains after this function is called for the second time
    if (iterationNum == 0){
        if (qIsNaN(UserVariables.diagonalRodLength)){
            UserVariables.diagonalRodLength = EEPROM.diagonalRod;
            emit signalLogConsole("Using default diagonal rod length from EEPROM",Qt::blue);
        }
    }

    tempAccuracy = (abs(Heights.X) + abs(Heights.XOpp) +
                    abs(Heights.Y) + abs(Heights.YOpp) +
                    abs(Heights.Z) + abs(Heights.ZOpp)) / 6.0;
    signalSetAccuracyPoint(iterationNum, tempAccuracy);
//    checkAccuracy(Heights.X,Heights.XOpp,Heights.Y,Heights.YOpp,Heights.Z,Heights.ZOpp);

    if (checkAccuracy(Heights.X,Heights.XOpp,Heights.Y,Heights.YOpp,Heights.Z,Heights.ZOpp)!=true){
//    if (calibrationState == true){
        towerOffsets (Heights.X,Heights.XOpp,Heights.Y,Heights.YOpp,Heights.Z,Heights.ZOpp);
        alphaRotation(Heights.X,Heights.XOpp,Heights.Y,Heights.YOpp,Heights.Z,Heights.ZOpp);
//        stepsPMM     (Heights.X,Heights.XOpp,Heights.Y,Heights.YOpp,Heights.Z,Heights.ZOpp);
        HRad         (Heights.X,Heights.XOpp,Heights.Y,Heights.YOpp,Heights.Z,Heights.ZOpp);
        return false;
    }
    else{
        //analyzeGeometry(ref Heights.X, ref Heights.XOpp, ref Heights.Y, ref Heights.YOpp, ref Heights.Z, ref Heights.ZOpp);
        return true;
    }
}

bool COpenDACT::basicCalibration()
{
    //check if eeprom object remains after this function is called for the second time
    if (iterationNum == 0){
        if (qIsNaN(UserVariables.diagonalRodLength)){
            UserVariables.diagonalRodLength = EEPROM.diagonalRod;
            emit signalLogConsole("Using default diagonal rod length from EEPROM",Qt::blue);
        }
    }

    tempAccuracy = (abs(Heights.X) + abs(Heights.XOpp) +
                    abs(Heights.Y) + abs(Heights.YOpp) +
                    abs(Heights.Z) + abs(Heights.ZOpp)) / 6;
    signalSetAccuracyPoint(iterationNum, tempAccuracy);
    //checkAccuracy(Heights.X,Heights.XOpp,Heights.Y,Heights.YOpp,Heights.Z,Heights.ZOpp);

    if (checkAccuracy(Heights.X,Heights.XOpp,Heights.Y,Heights.YOpp,Heights.Z,Heights.ZOpp)!=true){
//    if (calibrationState == true){
        bool spm = (Heights.X+Heights.Y+Heights.Z)/3 > (Heights.XOpp+Heights.YOpp+Heights.ZOpp)/3 + UserVariables.accuracy ||
                   (Heights.X+Heights.Y+Heights.Z)/3 < (Heights.XOpp+Heights.YOpp+Heights.ZOpp)/3 - UserVariables.accuracy;//returns false if drad does not need corrected

        bool tower = abs(qMax(Heights.X, qMax(Heights.Y, Heights.Z)) - qMin(Heights.X, qMin(Heights.Y, Heights.Z))) > UserVariables.accuracy;

        bool alpha = Heights.XOpp > Heights.YOpp + UserVariables.accuracy || Heights.XOpp < Heights.YOpp - UserVariables.accuracy ||
                     Heights.XOpp > Heights.ZOpp + UserVariables.accuracy || Heights.XOpp < Heights.ZOpp - UserVariables.accuracy ||
                     Heights.YOpp > Heights.XOpp + UserVariables.accuracy || Heights.YOpp < Heights.XOpp - UserVariables.accuracy ||
                     Heights.YOpp > Heights.ZOpp + UserVariables.accuracy || Heights.YOpp < Heights.ZOpp - UserVariables.accuracy ||
                     Heights.ZOpp > Heights.YOpp + UserVariables.accuracy || Heights.ZOpp < Heights.YOpp - UserVariables.accuracy ||
                     Heights.ZOpp > Heights.XOpp + UserVariables.accuracy || Heights.ZOpp < Heights.XOpp - UserVariables.accuracy;//returns false if tower offsets do not need corrected

        bool hrad =  Heights.X    < Heights.Y    + UserVariables.accuracy && Heights.X    > Heights.Y    - UserVariables.accuracy &&
                     Heights.X    < Heights.Z    + UserVariables.accuracy && Heights.X    > Heights.Z    - UserVariables.accuracy &&
                     Heights.X    < Heights.YOpp + UserVariables.accuracy && Heights.X    > Heights.YOpp - UserVariables.accuracy &&
                     Heights.X    < Heights.ZOpp + UserVariables.accuracy && Heights.X    > Heights.ZOpp - UserVariables.accuracy &&
                     Heights.X    < Heights.XOpp + UserVariables.accuracy && Heights.X    > Heights.XOpp - UserVariables.accuracy &&
                     Heights.XOpp < Heights.X    + UserVariables.accuracy && Heights.XOpp > Heights.X    - UserVariables.accuracy &&
                     Heights.XOpp < Heights.Y    + UserVariables.accuracy && Heights.XOpp > Heights.Y    - UserVariables.accuracy &&
                     Heights.XOpp < Heights.Z    + UserVariables.accuracy && Heights.XOpp > Heights.Z    - UserVariables.accuracy &&
                     Heights.XOpp < Heights.YOpp + UserVariables.accuracy && Heights.XOpp > Heights.YOpp - UserVariables.accuracy &&
                     Heights.XOpp < Heights.ZOpp + UserVariables.accuracy && Heights.XOpp > Heights.ZOpp - UserVariables.accuracy &&
                     Heights.Y    < Heights.X    + UserVariables.accuracy && Heights.Y    > Heights.X    - UserVariables.accuracy &&
                     Heights.Y    < Heights.Z    + UserVariables.accuracy && Heights.Y    > Heights.Z    - UserVariables.accuracy &&
                     Heights.Y    < Heights.XOpp + UserVariables.accuracy && Heights.Y    > Heights.XOpp - UserVariables.accuracy &&
                     Heights.Y    < Heights.YOpp + UserVariables.accuracy && Heights.Y    > Heights.YOpp - UserVariables.accuracy &&
                     Heights.Y    < Heights.ZOpp + UserVariables.accuracy && Heights.Y    > Heights.ZOpp - UserVariables.accuracy &&
                     Heights.YOpp < Heights.X    + UserVariables.accuracy && Heights.YOpp > Heights.X    - UserVariables.accuracy &&
                     Heights.YOpp < Heights.Y    + UserVariables.accuracy && Heights.YOpp > Heights.Y    - UserVariables.accuracy &&
                     Heights.YOpp < Heights.Z    + UserVariables.accuracy && Heights.YOpp > Heights.Z    - UserVariables.accuracy &&
                     Heights.YOpp < Heights.XOpp + UserVariables.accuracy && Heights.YOpp > Heights.XOpp - UserVariables.accuracy &&
                     Heights.YOpp < Heights.ZOpp + UserVariables.accuracy && Heights.YOpp > Heights.ZOpp - UserVariables.accuracy &&
                     Heights.Z    < Heights.X    + UserVariables.accuracy && Heights.Z    > Heights.X    - UserVariables.accuracy &&
                     Heights.Z    < Heights.Y    + UserVariables.accuracy && Heights.Z    > Heights.Y    - UserVariables.accuracy &&
                     Heights.Z    < Heights.XOpp + UserVariables.accuracy && Heights.Z    > Heights.XOpp - UserVariables.accuracy &&
                     Heights.Z    < Heights.YOpp + UserVariables.accuracy && Heights.Z    > Heights.YOpp - UserVariables.accuracy &&
                     Heights.Z    < Heights.ZOpp + UserVariables.accuracy && Heights.Z    > Heights.ZOpp - UserVariables.accuracy &&
                     Heights.ZOpp < Heights.X    + UserVariables.accuracy && Heights.ZOpp > Heights.X    - UserVariables.accuracy &&
                     Heights.ZOpp < Heights.Y    + UserVariables.accuracy && Heights.ZOpp > Heights.Y    - UserVariables.accuracy &&
                     Heights.ZOpp < Heights.Z    + UserVariables.accuracy && Heights.ZOpp > Heights.Z    - UserVariables.accuracy &&
                     Heights.ZOpp < Heights.XOpp + UserVariables.accuracy && Heights.ZOpp > Heights.XOpp - UserVariables.accuracy &&
                     Heights.ZOpp < Heights.YOpp + UserVariables.accuracy && Heights.ZOpp > Heights.YOpp - UserVariables.accuracy;

        emit signalLogConsole(QString("Tower:%1 SPM:%2 Alpha:%3 HRad:%4").arg(
                                  QString::number(tower),
                                  QString::number(spm),
                                  QString::number(alpha),
                                  QString::number(hrad)
                                  ));
        if (tower){
            towerOffsets(Heights.X,Heights.XOpp,Heights.Y,Heights.YOpp,Heights.Z,Heights.ZOpp);
        }
        else if (alpha){
            alphaRotation(Heights.X,Heights.XOpp,Heights.Y,Heights.YOpp,Heights.Z,Heights.ZOpp);
        }
        else if (spm){
            stepsPMM(Heights.X,Heights.XOpp,Heights.Y,Heights.YOpp,Heights.Z,Heights.ZOpp);
        }
        else if (hrad){
            HRad(Heights.X,Heights.XOpp,Heights.Y,Heights.YOpp,Heights.Z,Heights.ZOpp);
        }
    return false;
    }
    else{
        //analyzeGeometry(ref Heights.X, ref Heights.XOpp, ref Heights.Y, ref Heights.YOpp, ref Heights.Z, ref Heights.ZOpp);
        return true;
    }
}


bool COpenDACT::checkAccuracy(float X, float XOpp, float Y, float YOpp, float Z, float ZOpp)
{
    float accuracy = UserVariables.accuracy;

    if (abs(X)<=accuracy && abs(XOpp)<=accuracy &&
        abs(Y)<=accuracy && abs(YOpp)<=accuracy &&
        abs(Z)<=accuracy && abs(ZOpp)<=accuracy){
        if (UserVariables.probeChoice == "FSR"){
            EEPROM.zMaxLength -= UserVariables.FSROffset;
            emit signalLogConsole("Setting Z Max Length with adjustment for FSR");
        }
        return true;
        //calibrationState = false;
    }
    else{
//        checkHeights = true;
        emit signalLogConsole("Continuing Calibration");
        return false;
    }
}

void COpenDACT::HRad(float& X,float& XOpp,float& Y,float& YOpp,float& Z,float& ZOpp)
{
    float HRadSA = ((X + XOpp + Y + YOpp + Z + ZOpp) / 6);
    float HRadRatio = UserVariables.HRadRatio;

    EEPROM.HRadius += (HRadSA / HRadRatio);

    X -= HRadSA;
    Y -= HRadSA;
    Z -= HRadSA;
    XOpp -= HRadSA;
    YOpp -= HRadSA;
    ZOpp -= HRadSA;

    emit signalLogConsole(QString("HRad: %1.").arg(QString::number(EEPROM.HRadius,'f',3)));
}

void COpenDACT::towerOffsets (float& X,float& XOpp,float& Y,float& YOpp,float& Z,float& ZOpp)
{
    int j = 0;
//    float accuracy   = UserVariables.calculationAccuracy; // UNUSED
    float tempX2     = X;
    float tempXOpp2  = XOpp;
    float tempY2     = Y;
    float tempYOpp2  = YOpp;
    float tempZ2     = Z;
    float tempZOpp2  = ZOpp;
    float offsetX    = EEPROM.offsetX;
    float offsetY    = EEPROM.offsetY;
    float offsetZ    = EEPROM.offsetZ;
    float stepsPerMM = EEPROM.stepsPerMM;

    float towMain = UserVariables.offsetCorrection;//0.6
    float oppMain = UserVariables.mainOppPerc;//0.5
    float towSub = UserVariables.towPerc;//0.3
    float oppSub = UserVariables.oppPerc;//-0.25

    while (j < 100){
        if (abs(tempX2) > UserVariables.accuracy ||
            abs(tempY2) > UserVariables.accuracy ||
            abs(tempZ2) > UserVariables.accuracy){
            offsetX   -= tempX2 * stepsPerMM * (1 / towMain);

            tempXOpp2 -= tempX2 * ( oppMain / towMain);
            tempY2    -= tempX2 * ( towSub  / towMain);
            tempYOpp2 -= tempX2 * (-oppSub  / towMain);
            tempZ2    -= tempX2 * ( towSub  / towMain);
            tempZOpp2 -= tempX2 * (-oppSub  / towMain);
            tempX2    -= tempX2 / 1;

            offsetY   -= tempY2 * stepsPerMM * (1 / towMain);

            tempYOpp2 -= tempY2 * ( oppMain / towMain);
            tempX2    -= tempY2 * ( towSub  / towMain);
            tempXOpp2 -= tempY2 * (-oppSub  / towMain);
            tempZ2    -= tempY2 * ( towSub  / towMain);
            tempZOpp2 -= tempY2 * (-oppSub  / towMain);
            tempY2    -= tempY2 / 1;

            offsetZ   -= tempZ2 * stepsPerMM * (1 / towMain);

            tempZOpp2 -= tempZ2 * ( oppMain / towMain);
            tempX2    -= tempZ2 * ( towSub  / towMain);
            tempXOpp2 += tempZ2 * (-oppSub  / towMain);
            tempY2    -= tempZ2 * ( towSub  / towMain);
            tempYOpp2 -= tempZ2 * (-oppSub  / towMain);
            tempZ2    -= tempZ2 / 1;

            tempX2    = checkZero(tempX2);
            tempY2    = checkZero(tempY2);
            tempZ2    = checkZero(tempZ2);
            tempXOpp2 = checkZero(tempXOpp2);
            tempYOpp2 = checkZero(tempYOpp2);
            tempZOpp2 = checkZero(tempZOpp2);

            if (abs(tempX2) <= UserVariables.accuracy &&
                abs(tempY2) <= UserVariables.accuracy &&
                abs(tempZ2) <= UserVariables.accuracy){
                emit signalLogConsole(QString("VHeights :%1, %2, %3, %4, %5, %6").arg(
                                      QString::number(tempX2   ,'f',3),
                                      QString::number(tempXOpp2,'f',3),
                                      QString::number(tempY2   ,'f',3),
                                      QString::number(tempYOpp2,'f',3),
                                      QString::number(tempZ2   ,'f',3),
                                      QString::number(tempZOpp2,'f',3)));
                emit signalLogConsole(QString("Offs :%1, %2, %3").arg(
                                          QString::number(offsetX,'f',3),
                                          QString::number(offsetY,'f',3),
                                          QString::number(offsetZ,'f',3)));
                emit signalLogConsole("No Hrad correction");
                float smallest = qMin(offsetX, qMin(offsetY, offsetZ));
                offsetX -= smallest;
                offsetY -= smallest;
                offsetZ -= smallest;
                emit signalLogConsole(QString("Offs :%1, %2, %3").arg(
                                          QString::number(offsetX,'f',3),
                                          QString::number(offsetY,'f',3),
                                          QString::number(offsetZ,'f',3)));
                X    = tempX2;
                XOpp = tempXOpp2;
                Y    = tempY2;
                YOpp = tempYOpp2;
                Z    = tempZ2;
                ZOpp = tempZOpp2;

                //round to the nearest whole number
                EEPROM.offsetX = int(offsetX+0.5);
                EEPROM.offsetY = int(offsetY+0.5);
                EEPROM.offsetZ = int(offsetZ+0.5);

                j = 100;
            }
            else if (j == 99){
                emit signalLogConsole(QString("VHeights :%1, %2, %3, %4, %5, %6").arg(
                                      QString::number(tempX2   ,'f',3),
                                      QString::number(tempXOpp2,'f',3),
                                      QString::number(tempY2   ,'f',3),
                                      QString::number(tempYOpp2,'f',3),
                                      QString::number(tempZ2   ,'f',3),
                                      QString::number(tempZOpp2,'f',3)));
                emit signalLogConsole(QString("Offs :%1, %2, %3").arg(
                                          QString::number(offsetX,'f',3),
                                          QString::number(offsetY,'f',3),
                                          QString::number(offsetZ,'f',3)));
                float dradCorr = tempX2 * -1.25;
                float HRadRatio = UserVariables.HRadRatio;

                EEPROM.HRadius += dradCorr;

                EEPROM.offsetX = 0;
                EEPROM.offsetY = 0;
                EEPROM.offsetZ = 0;

                //hradsa = dradcorr
                //solve inversely from previous method
                float HRadOffset = HRadRatio * dradCorr;

                tempX2    -= HRadOffset;
                tempY2    -= HRadOffset;
                tempZ2    -= HRadOffset;
                tempXOpp2 -= HRadOffset;
                tempYOpp2 -= HRadOffset;
                tempZOpp2 -= HRadOffset;

                emit signalLogConsole(QString("Hrad correction: %1").arg(QString::number(dradCorr,'f',3)));
                emit signalLogConsole(QString("HRad: %1").arg(QString::number(EEPROM.HRadius,'f',3)));
                j = 0;
            }
            else{
                j++;
            }

            //UserInterface.logConsole("Offs :" + offsetX + " " + offsetY + " " + offsetZ);
            //UserInterface.logConsole("VHeights :" + tempX2 + " " + tempXOpp2 + " " + tempY2 + " " + tempYOpp2 + " " + tempZ2 + " " + tempZOpp2);
        }
        else{
            j = 100;
            emit signalLogConsole("Tower Offsets and Delta Radii Calibrated");
        }
    }

    if (EEPROM.offsetX > 1000 || EEPROM.offsetY > 1000 || EEPROM.offsetZ > 1000){
        emit signalLogConsole("Tower offset calibration error, setting default values.",Qt::red);
        emit signalLogConsole(QString("Tower offsets before damage prevention: X%1 Y%2 Z%3").arg(
                                  QString::number(offsetX,'f',3),
                                  QString::number(offsetY,'f',3),
                                  QString::number(offsetZ,'f',3)));
        offsetX = 0;
        offsetY = 0;
        offsetZ = 0;
    }

}

void COpenDACT::alphaRotation(float& X,float& XOpp,float& Y,float& YOpp,float& Z,float& ZOpp)
{
    Q_UNUSED(X)
    Q_UNUSED(Y)
    Q_UNUSED(Z)
//    float offsetX = EEPROM.offsetX; // UNUSED
//    float offsetY = EEPROM.offsetY; // UNUSED
//    float offsetZ = EEPROM.offsetZ; // UNUSED
//    float accuracy = UserVariables.accuracy; // UNUSED

    //change to object
    float alphaRotationPercentage = UserVariables.alphaRotationPercentage;

    int k = 0;
    while (k < 100){
        //X Alpha Rotation
        if (YOpp > ZOpp){
            float ZYOppAvg = (YOpp - ZOpp) / 2;
            EEPROM.A = EEPROM.A + (ZYOppAvg * alphaRotationPercentage); // (0.5/((diff y0 and z0 at X + 0.5)-(diff y0 and z0 at X = 0))) * 2 = 1.75
            YOpp = YOpp - ZYOppAvg;
            ZOpp = ZOpp + ZYOppAvg;
        }
        else if (YOpp < ZOpp){
            float ZYOppAvg = (ZOpp - YOpp) / 2;
            EEPROM.A = EEPROM.A - (ZYOppAvg * alphaRotationPercentage);
            YOpp = YOpp + ZYOppAvg;
            ZOpp = ZOpp - ZYOppAvg;
        }

        //Y Alpha Rotation
        if (ZOpp > XOpp){
            float XZOppAvg = (ZOpp - XOpp) / 2;
            EEPROM.B = EEPROM.B + (XZOppAvg * alphaRotationPercentage);
            ZOpp = ZOpp - XZOppAvg;
            XOpp = XOpp + XZOppAvg;
        }
        else if (ZOpp < XOpp){
            float XZOppAvg = (XOpp - ZOpp) / 2;
            EEPROM.B = EEPROM.B - (XZOppAvg * alphaRotationPercentage);
            ZOpp = ZOpp + XZOppAvg;
            XOpp = XOpp - XZOppAvg;
        }
        //Z Alpha Rotation
        if (XOpp > YOpp){
            float YXOppAvg = (XOpp - YOpp) / 2;
            EEPROM.C = EEPROM.C + (YXOppAvg * alphaRotationPercentage);
            XOpp = XOpp - YXOppAvg;
            YOpp = YOpp + YXOppAvg;
        }
        else if (XOpp < YOpp){
            float YXOppAvg = (YOpp - XOpp) / 2;
            EEPROM.C = EEPROM.C - (YXOppAvg * alphaRotationPercentage);
            XOpp = XOpp + YXOppAvg;
            YOpp = YOpp - YXOppAvg;
        }

        //determine if value is close enough
        float hTow = qMax(qMax(XOpp, YOpp), ZOpp);
        float lTow = qMin(qMin(XOpp, YOpp), ZOpp);
        float towDiff = hTow - lTow;

        if (towDiff < UserVariables.calculationAccuracy && towDiff > -UserVariables.calculationAccuracy){
            k = 100;
            //log
            emit signalLogConsole(QString("ABC: X%1 Y%2 Z%3").arg(
                                      QString::number(EEPROM.A,'f',3),
                                      QString::number(EEPROM.B,'f',3),
                                      QString::number(EEPROM.C,'f',3)));
        }
        else
            k++;
    }
}

void COpenDACT::stepsPMM (float& X,float& XOpp,float& Y,float& YOpp,float& Z,float& ZOpp)
{
    /*
    float diagChange = 1 / UserVariables.deltaOpp;
    float towChange = 1 / UserVariables.deltaTower;
    */
    float diagChange = 1 / UserVariables.deltaOpp;
    float towChange = 1 / UserVariables.deltaTower;

    float XYZ = (X + Y + Z) / 3;
    float XYZOpp = (XOpp + YOpp + ZOpp) / 3;

    float spmm = EEPROM.stepsPerMM - (XYZ - XYZOpp) * ((diagChange + towChange) / 2);
    qDebug() << "stepsPMM" << spmm;
    if (abs(spmm-100)<5.0){
        EEPROM.stepsPerMM = spmm;//-= (XYZ - XYZOpp) * ((diagChange + towChange) / 2);

        //XYZ is increased by the offset

        X += (XYZ - XYZOpp) * diagChange * 1;
        Y += (XYZ - XYZOpp) * diagChange * 1;
        Z += (XYZ - XYZOpp) * diagChange * 1;
        XOpp += (XYZ - XYZOpp) * towChange * 0.75f;
        YOpp += (XYZ - XYZOpp) * towChange * 0.75f;
        ZOpp += (XYZ - XYZOpp) * towChange * 0.75f;
        emit signalLogConsole(QString("Steps per Millimeter: %1").arg(QString::number(EEPROM.stepsPerMM,'f',3)));
    }
    else{
        emit signalLogConsole(QString("Steps per Millimeter wrong: %1. No change.").arg(QString::number(spmm,'f',3)),Qt::red);
    }

}

void COpenDACT::heuristicLearning()
{
    qDebug() <<"heuristicLearning()";
    //find base heights
    //find heights with each value increased by 1 - HRad, tower offset 1-3, diagonal rod

    if (UserVariables.advancedCalCount == 0){//start
//        if (Connection._serialPort.IsOpen){
            EEPROM.stepsPerMM += 1;
            emit signalLogConsole(QString("Setting steps per millimeter to: %1").arg(QString::number(EEPROM.stepsPerMM,'f',3)));
//            UserInterface.logConsole("Setting steps per millimeter to: " + (EEPROM.stepsPerMM).ToString());
//        }

        //check heights

        UserVariables.advancedCalCount++;
    }
    else if (UserVariables.advancedCalCount == 1){//get diagonal rod percentages

        UserVariables.deltaTower = ((Heights.teX - Heights.X) + (Heights.teY - Heights.Y) + (Heights.teZ - Heights.Z)) / 3;
        UserVariables.deltaOpp = ((Heights.teXOpp - Heights.XOpp) + (Heights.teYOpp - Heights.YOpp) + (Heights.teZOpp - Heights.ZOpp)) / 3;

//        if (Connection._serialPort.IsOpen){
            EEPROM.stepsPerMM -= 1;
//            UserInterface.logConsole("Setting steps per millimeter to: " + (EEPROM.stepsPerMM).ToString());
            emit signalLogConsole(QString("Setting steps per millimeter to: %1").arg(QString::number(EEPROM.stepsPerMM,'f',3)));

            //set Hrad +1
            EEPROM.HRadius += 1;
//            UserInterface.logConsole("Setting horizontal radius to: " + (EEPROM.HRadius).ToString());
            emit signalLogConsole(QString("Setting horizontal radius to: %1").arg(QString::number(EEPROM.HRadius,'f',3)));
//        }

        //check heights

        UserVariables.advancedCalCount++;
    }
    else if (UserVariables.advancedCalCount == 2) {//get HRad percentages
        UserVariables.HRadRatio = -(abs((Heights.X - Heights.teX) + (Heights.Y - Heights.teY) + (Heights.Z - Heights.teZ) + (Heights.XOpp - Heights.teXOpp) + (Heights.YOpp - Heights.teYOpp) + (Heights.ZOpp - Heights.teZOpp))) / 6;

//        if (Connection._serialPort.IsOpen)        {
            //reset horizontal radius
            EEPROM.HRadius -= 1;
//            UserInterface.logConsole("Setting horizontal radius to: " + (EEPROM.HRadius).ToString());
            emit signalLogConsole(QString("Setting horizontal radius to: %1").arg(QString::number(EEPROM.HRadius,'f',3)));

            //set X offset
            EEPROM.offsetX += 80;
//            UserInterface.logConsole("Setting offset X to: " + (EEPROM.offsetX).ToString());
            emit signalLogConsole(QString("Setting offset X to: %1").arg(QString::number(EEPROM.offsetX,'f',3)));
//        }

        //check heights
        UserVariables.advancedCalCount++;
    }
    else if (UserVariables.advancedCalCount == 3) {//get X offset percentages

        UserVariables.offsetCorrection += abs(1 / (Heights.X - Heights.teX));
        UserVariables.mainOppPerc += abs((Heights.XOpp - Heights.teXOpp) / (Heights.X - Heights.teX));
        UserVariables.towPerc += (abs((Heights.Y - Heights.teY) / (Heights.X - Heights.teX)) +
                                  abs((Heights.Z - Heights.teZ) / (Heights.X - Heights.teX))) / 2;
        UserVariables.oppPerc += (abs((Heights.YOpp - Heights.teYOpp) / (Heights.X - Heights.teX)) +
                                  abs((Heights.ZOpp - Heights.teZOpp) / (Heights.X - Heights.teX))) / 2;

//        if (Connection._serialPort.IsOpen)
//        {
            //reset X offset
            EEPROM.offsetX -= 80;
//            UserInterface.logConsole("Setting offset X to: " + (EEPROM.offsetX).ToString());
            emit signalLogConsole(QString("Setting offset X to: %1").arg(QString::number(EEPROM.offsetX,'f',3)));

            //set Y offset
            EEPROM.offsetY += 80;
//            UserInterface.logConsole("Setting offset Y to: " + (EEPROM.offsetY).ToString());
            emit signalLogConsole(QString("Setting offset Y to: %1").arg(QString::number(EEPROM.offsetY,'f',3)));
//        }

        //check heights

        UserVariables.advancedCalCount++;
    }
    else if (UserVariables.advancedCalCount == 4)  {//get Y offset percentages

        UserVariables.offsetCorrection += abs(1 / (Heights.Y - Heights.teY));
        UserVariables.mainOppPerc += abs((Heights.YOpp - Heights.teYOpp) / (Heights.Y - Heights.teY));
        UserVariables.towPerc += (abs((Heights.X - Heights.teX) / (Heights.Y - Heights.teY)) +
                                  abs((Heights.Z - Heights.teZ) / (Heights.Y - Heights.teY))) / 2;
        UserVariables.oppPerc += (abs((Heights.XOpp - Heights.teXOpp) / (Heights.Y - Heights.teY)) +
                                  abs((Heights.ZOpp - Heights.teZOpp) / (Heights.Y - Heights.teY))) / 2;

//        if (Connection._serialPort.IsOpen)    {
            //reset Y offset
            EEPROM.offsetY -= 80;
//            UserInterface.logConsole("Setting offset Y to: " + (EEPROM.offsetY).ToString());
            emit signalLogConsole(QString("Setting offset Y to: %1").arg(QString::number(EEPROM.offsetY,'f',3)));

            //set Z offset
            EEPROM.offsetZ += 80;
//            UserInterface.logConsole("Setting offset Z to: " + (EEPROM.offsetZ).ToString());
            emit signalLogConsole(QString("Setting offset Z to: %1").arg(QString::number(EEPROM.offsetZ,'f',3)));
//        }

        //check heights

        UserVariables.advancedCalCount++;
    }
    else if (UserVariables.advancedCalCount == 5)    {//get Z offset percentages

        UserVariables.offsetCorrection += abs(1 / (Heights.Z - Heights.teZ));
        UserVariables.mainOppPerc += abs((Heights.ZOpp - Heights.teZOpp) / (Heights.Z - Heights.teZ));
        UserVariables.towPerc += (abs((Heights.X - Heights.teX) / (Heights.Z - Heights.teZ)) +
                                  abs((Heights.Y - Heights.teY) / (Heights.Z - Heights.teZ))) / 2;
        UserVariables.oppPerc += (abs((Heights.XOpp - Heights.teXOpp) / (Heights.Z - Heights.teZ)) +
                                  abs((Heights.YOpp - Heights.teYOpp) / (Heights.Z - Heights.teZ))) / 2;

//        if (Connection._serialPort.IsOpen)        {
            //set Z offset
            EEPROM.offsetZ -= 80;
//            UserInterface.logConsole("Setting offset Z to: " + (EEPROM.offsetZ).ToString());
            emit signalLogConsole(QString("Setting offset Z to: %1").arg(QString::number(EEPROM.offsetZ,'f',3)));

            //set alpha rotation offset perc X
            EEPROM.A += 1;
//            UserInterface.logConsole("Setting Alpha A to: " + (EEPROM.A).ToString());
            emit signalLogConsole(QString("Setting Alpha A to: %1").arg(QString::number(EEPROM.A,'f',3)));
//        }

        //check heights
        UserVariables.advancedCalCount++;
    }
    else if (UserVariables.advancedCalCount == 6)    {//6
        //get A alpha rotation

        UserVariables.alphaRotationPercentage += (2 / abs((Heights.YOpp - Heights.ZOpp) - (Heights.teYOpp - Heights.teZOpp)));

//        if (Connection._serialPort.IsOpen)        {
            //set alpha rotation offset perc X
            EEPROM.A -= 1;
//            UserInterface.logConsole("Setting Alpha A to: " + (EEPROM.A).ToString());
            emit signalLogConsole(QString("Setting Alpha A to: %1").arg(QString::number(EEPROM.A,'f',3)));

            //set alpha rotation offset perc Y
            EEPROM.B += 1;
//            UserInterface.logConsole("Setting Alpha B to: " + (EEPROM.B).ToString());
            emit signalLogConsole(QString("Setting Alpha B to: %1").arg(QString::number(EEPROM.B,'f',3)));
//        }

        //check heights

        UserVariables.advancedCalCount++;
    }
    else if (UserVariables.advancedCalCount == 7)    {//get B alpha rotation //7
        UserVariables.alphaRotationPercentage += (2 / abs((Heights.ZOpp - Heights.XOpp) - (Heights.teXOpp - Heights.teXOpp)));

//        if (Connection._serialPort.IsOpen)        {
            //set alpha rotation offset perc Y
            EEPROM.B -= 1;
//            UserInterface.logConsole("Setting Alpha B to: " + (EEPROM.B).ToString());
            emit signalLogConsole(QString("Setting Alpha B to: %1").arg(QString::number(EEPROM.B,'f',3)));

            //set alpha rotation offset perc Z
            EEPROM.C += 1;
//            UserInterface.logConsole("Setting Alpha C to: " + (EEPROM.C).ToString());
            emit signalLogConsole(QString("Setting Alpha C to: %1").arg(QString::number(EEPROM.C,'f',3)));
//        }

        //check heights

        UserVariables.advancedCalCount++;
    }
    else if (UserVariables.advancedCalCount == 8) {//8//get C alpha rotation

        UserVariables.alphaRotationPercentage += (2 / abs((Heights.XOpp - Heights.YOpp) - (Heights.teXOpp - Heights.teYOpp)));
        UserVariables.alphaRotationPercentage /= 3;

//        if (Connection._serialPort.IsOpen)    {
            //set alpha rotation offset perc Z
            EEPROM.C -= 1;
//            UserInterface.logConsole("Setting Alpha C to: " + (EEPROM.C).ToString());
            emit signalLogConsole(QString("Setting Alpha C to: %1").arg(QString::number(EEPROM.C,'f',3)));

//        }

//        UserInterface.logConsole("Alpha offset percentage: " + UserVariables.alphaRotationPercentage);
        emit signalLogConsole(QString("Alpha offset percentage: %1").arg(QString::number(UserVariables.alphaRotationPercentage,'f',3)));

        UserVariables.advancedCalibration = false;
//        Program.mainFormTest.setButtonValues();
        UserVariables.advancedCalCount = 0;
        isHeuristicComplete = true;

        //check heights

    }

    checkHeights = true;
}

