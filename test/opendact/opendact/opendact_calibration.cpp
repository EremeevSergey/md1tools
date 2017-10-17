#include <math.h>
#include "opendact_calibration.h"
#include "opendact.h"

#define FS(val) (QString::number(val,'f',3))

//CCalibration::CCalibration()
//{
//    calibrateInProgress = false;
//    calibrationState = false;
//    calibrationComplete = false;
//    calibrationSelection = 0;
//    iterationNum = 0;
//}


//void CCalibration::calibrate()
//{
//    if (calibrationSelection == 0){
//        basicCalibration();
//    }
//    iterationNum++;
//}

//void CCalibration::basicCalibration()
//{
//    //check if eeprom object remains after this function is called for the second time
///*
//    if (iterationNum == 0){
//        if (UserVariables.diagonalRodLength.ToString() == ""){
//            UserVariables.diagonalRodLength = EEPROM.diagonalRod;
//            emit signalLogConsole("Using default diagonal rod length from EEPROM");
//        }
//    }

//    tempAccuracy = (abs(Heights.X) + abs(Heights.XOpp) + abs(Heights.Y) + abs(Heights.YOpp) +
//                    abs(Heights.Z) + abs(Heights.ZOpp)) / 6.0;
//    Program.mainFormTest.setAccuracyPoint(iterationNum, tempAccuracy);
//    checkAccuracy(Heights.X,Heights.XOpp,Heights.Y,Heights.YOpp,Heights.Z,Heights.ZOpp);

//    if (calibrationState == true)
//    {
//        ////UserInterface.logConsole(test.ToString());

//        bool spm = (Heights.X + Heights.Y + Heights.Z) / 3 > (Heights.XOpp + Heights.YOpp + Heights.ZOpp) / 3 + UserVariables.accuracy || (Heights.X + Heights.Y + Heights.Z) / 3 < (Heights.XOpp + Heights.YOpp + Heights.ZOpp) / 3 - UserVariables.accuracy;//returns false if drad does not need corrected
//        QString  str_spm="0";
//        if (spm) str_spm="1";
//        bool tower = Heights.X > Heights.Y + UserVariables.accuracy || Heights.X < Heights.Y - UserVariables.accuracy || Heights.X > Heights.Z + UserVariables.accuracy || Heights.X < Heights.Z - UserVariables.accuracy ||
//                     Heights.Y > Heights.X + UserVariables.accuracy || Heights.Y < Heights.X - UserVariables.accuracy || Heights.Y > Heights.Z + UserVariables.accuracy || Heights.Y < Heights.Z - UserVariables.accuracy ||
//                     Heights.Z > Heights.Y + UserVariables.accuracy || Heights.Z < Heights.Y - UserVariables.accuracy || Heights.Z > Heights.X + UserVariables.accuracy || Heights.Z < Heights.X - UserVariables.accuracy;//returns false if tower offsets do not need corrected
//        QString    str_tower="0";
//        if (tower) str_tower="1";

//        bool alpha = Heights.XOpp > Heights.YOpp + UserVariables.accuracy || Heights.XOpp < Heights.YOpp - UserVariables.accuracy || Heights.XOpp > Heights.ZOpp + UserVariables.accuracy || Heights.XOpp < Heights.ZOpp - UserVariables.accuracy ||
//                     Heights.YOpp > Heights.XOpp + UserVariables.accuracy || Heights.YOpp < Heights.XOpp - UserVariables.accuracy || Heights.YOpp > Heights.ZOpp + UserVariables.accuracy || Heights.YOpp < Heights.ZOpp - UserVariables.accuracy ||
//                     Heights.ZOpp > Heights.YOpp + UserVariables.accuracy || Heights.ZOpp < Heights.YOpp - UserVariables.accuracy || Heights.ZOpp > Heights.XOpp + UserVariables.accuracy || Heights.ZOpp < Heights.XOpp - UserVariables.accuracy;//returns false if tower offsets do not need corrected
//        QString    str_alpha="0";
//        if (alpha) str_alpha="1";

//        bool hrad =  Heights.X    < Heights.Y    + UserVariables.accuracy && Heights.X    > Heights.Y    - UserVariables.accuracy &&
//                     Heights.X    < Heights.Z    + UserVariables.accuracy && Heights.X    > Heights.Z    - UserVariables.accuracy &&
//                     Heights.X    < Heights.YOpp + UserVariables.accuracy && Heights.X    > Heights.YOpp - UserVariables.accuracy &&
//                     Heights.X    < Heights.ZOpp + UserVariables.accuracy && Heights.X    > Heights.ZOpp - UserVariables.accuracy &&
//                     Heights.X    < Heights.XOpp + UserVariables.accuracy && Heights.X    > Heights.XOpp - UserVariables.accuracy &&
//                     Heights.XOpp < Heights.X    + UserVariables.accuracy && Heights.XOpp > Heights.X    - UserVariables.accuracy &&
//                     Heights.XOpp < Heights.Y    + UserVariables.accuracy && Heights.XOpp > Heights.Y    - UserVariables.accuracy &&
//                     Heights.XOpp < Heights.Z    + UserVariables.accuracy && Heights.XOpp > Heights.Z    - UserVariables.accuracy &&
//                     Heights.XOpp < Heights.YOpp + UserVariables.accuracy && Heights.XOpp > Heights.YOpp - UserVariables.accuracy &&
//                     Heights.XOpp < Heights.ZOpp + UserVariables.accuracy && Heights.XOpp > Heights.ZOpp - UserVariables.accuracy &&
//                     Heights.Y    < Heights.X    + UserVariables.accuracy && Heights.Y    > Heights.X    - UserVariables.accuracy &&
//                     Heights.Y    < Heights.Z    + UserVariables.accuracy && Heights.Y    > Heights.Z    - UserVariables.accuracy &&
//                     Heights.Y    < Heights.XOpp + UserVariables.accuracy && Heights.Y    > Heights.XOpp - UserVariables.accuracy &&
//                     Heights.Y    < Heights.YOpp + UserVariables.accuracy && Heights.Y    > Heights.YOpp - UserVariables.accuracy &&
//                     Heights.Y    < Heights.ZOpp + UserVariables.accuracy && Heights.Y    > Heights.ZOpp - UserVariables.accuracy &&
//                     Heights.YOpp < Heights.X    + UserVariables.accuracy && Heights.YOpp > Heights.X    - UserVariables.accuracy &&
//                     Heights.YOpp < Heights.Y    + UserVariables.accuracy && Heights.YOpp > Heights.Y    - UserVariables.accuracy &&
//                     Heights.YOpp < Heights.Z    + UserVariables.accuracy && Heights.YOpp > Heights.Z    - UserVariables.accuracy &&
//                     Heights.YOpp < Heights.XOpp + UserVariables.accuracy && Heights.YOpp > Heights.XOpp - UserVariables.accuracy &&
//                     Heights.YOpp < Heights.ZOpp + UserVariables.accuracy && Heights.YOpp > Heights.ZOpp - UserVariables.accuracy &&
//                     Heights.Z    < Heights.X    + UserVariables.accuracy && Heights.Z    > Heights.X    - UserVariables.accuracy &&
//                     Heights.Z    < Heights.Y    + UserVariables.accuracy && Heights.Z    > Heights.Y    - UserVariables.accuracy &&
//                     Heights.Z    < Heights.XOpp + UserVariables.accuracy && Heights.Z    > Heights.XOpp - UserVariables.accuracy &&
//                     Heights.Z    < Heights.YOpp + UserVariables.accuracy && Heights.Z    > Heights.YOpp - UserVariables.accuracy &&
//                     Heights.Z    < Heights.ZOpp + UserVariables.accuracy && Heights.Z    > Heights.ZOpp - UserVariables.accuracy &&
//                     Heights.ZOpp < Heights.X    + UserVariables.accuracy && Heights.ZOpp > Heights.X    - UserVariables.accuracy &&
//                     Heights.ZOpp < Heights.Y    + UserVariables.accuracy && Heights.ZOpp > Heights.Y    - UserVariables.accuracy &&
//                     Heights.ZOpp < Heights.Z    + UserVariables.accuracy && Heights.ZOpp > Heights.Z    - UserVariables.accuracy &&
//                     Heights.ZOpp < Heights.XOpp + UserVariables.accuracy && Heights.ZOpp > Heights.XOpp - UserVariables.accuracy &&
//                     Heights.ZOpp < Heights.YOpp + UserVariables.accuracy && Heights.ZOpp > Heights.YOpp - UserVariables.accuracy;
//        QString   str_hrad="0";
//        if (hrad) str_hrad="1";

//        signalLogConsole("Tower:" + str_tower + " SPM:" + str_spm + " Alpha:" + str_alpha + " HRad:" + str_hrad);
////        signalLogConsole("Tower:" + tower + " SPM:" + spm + " Alpha:" + alpha + " HRad:" + hrad);

//        if (tower){
//            towerOffsets(Heights.X,Heights.XOpp,Heights.Y,Heights.YOpp,Heights.Z,Heights.ZOpp);
//        }
//        else if (alpha){
//            alphaRotation(Heights.X,Heights.XOpp,Heights.Y,Heights.YOpp,Heights.Z,Heights.ZOpp);
//        }
//        else if (spm){
//            stepsPMM(Heights.X,Heights.XOpp,Heights.Y,Heights.YOpp,Heights.Z,Heights.ZOpp);
//        }
//        else if (hrad){
//            HRad(Heights.X,Heights.XOpp,Heights.Y,Heights.YOpp,Heights.Z,Heights.ZOpp);
//        }

//    }
//    else{
//        ////analyzeGeometry(ref Heights.X, ref Heights.XOpp, ref Heights.Y, ref Heights.YOpp, ref Heights.Z, ref Heights.ZOpp);
//    }
//    */
//}

//void CCalibration::checkAccuracy(float& X,float& XOpp, float& Y,float& YOpp,float& Z,float& ZOpp)
//{
///*    float accuracy = UserVariables.accuracy;

//    if (X    <= accuracy && X    >= -accuracy &&
//        XOpp <= accuracy && XOpp >= -accuracy &&
//        Y    <= accuracy && Y    >= -accuracy &&
//        YOpp <= accuracy && YOpp >= -accuracy &&
//        Z    <= accuracy && Z    >= -accuracy &&
//        ZOpp <= accuracy && ZOpp >= -accuracy){
//        if (UserVariables.probeChoice == "FSR"){
//            EEPROM.zMaxLength -= UserVariables.FSROffset;
//            signalLogConsole("Setting Z Max Length with adjustment for FSR");
//        }

//        calibrationState = false;
//    }
//    else{
//        GCode.checkHeights = true;
//        signalLogConsole("Continuing Calibration");
//    }
//    */
//}

//void CCalibration::HRad(float& X,float& XOpp,float& Y,float& YOpp,float& Z,float& ZOpp)
//{
//    float HRadSA = ((X + XOpp + Y + YOpp + Z + ZOpp) / 6.0);
//    float HRadRatio = UserVariables.HRadRatio;

//    EEPROM.HRadius += (HRadSA / HRadRatio);

//    X -= HRadSA;
//    Y -= HRadSA;
//    Z -= HRadSA;
//    XOpp -= HRadSA;
//    YOpp -= HRadSA;
//    ZOpp -= HRadSA;

//    signalLogConsole("HRad:" + FS(EEPROM.HRadius));
//}

//void CCalibration::towerOffsets(float& X,float& XOpp,float& Y,float& YOpp,float& Z,float& ZOpp)
//{
///*
//    int j = 0;
//    float accuracy = UserVariables.calculationAccuracy;
//    float tempX2 = X;
//    float tempXOpp2 = XOpp;
//    float tempY2 = Y;
//    float tempYOpp2 = YOpp;
//    float tempZ2 = Z;
//    float tempZOpp2 = ZOpp;
//    float offsetX = EEPROM.offsetX;
//    float offsetY = EEPROM.offsetY;
//    float offsetZ = EEPROM.offsetZ;
//    float stepsPerMM = EEPROM.stepsPerMM;

//    float towMain = UserVariables.offsetCorrection;//0.6
//    float oppMain = UserVariables.mainOppPerc;//0.5
//    float towSub = UserVariables.towPerc;//0.3
//    float oppSub = UserVariables.oppPerc;//-0.25

//    while (j < 100){
//        if (abs(tempX2) > UserVariables.accuracy ||
//            abs(tempY2) > UserVariables.accuracy ||
//            abs(tempZ2) > UserVariables.accuracy){
//            offsetX   -= tempX2 * stepsPerMM * (1.0 / towMain);

//            tempXOpp2 -= tempX2 * ( oppMain / towMain);
//            tempY2    -= tempX2 * ( towSub  / towMain);
//            tempYOpp2 -= tempX2 * (-oppSub  / towMain);
//            tempZ2    -= tempX2 * ( towSub  / towMain);
//            tempZOpp2 -= tempX2 * (-oppSub  / towMain);
//            tempX2    -= tempX2 / 1;

//            offsetY -= tempY2 * stepsPerMM * (1 / towMain);

//            tempYOpp2 -= tempY2 * ( oppMain / towMain);
//            tempX2    -= tempY2 * ( towSub  / towMain);
//            tempXOpp2 -= tempY2 * (-oppSub  / towMain);
//            tempZ2    -= tempY2 * ( towSub  / towMain);
//            tempZOpp2 -= tempY2 * (-oppSub  / towMain);
//            tempY2    -= tempY2 / 1;

//            offsetZ   -= tempZ2 * stepsPerMM * (1 / towMain);

//            tempZOpp2 -= tempZ2 * ( oppMain / towMain);
//            tempX2    -= tempZ2 * ( towSub  / towMain);
//            tempXOpp2 += tempZ2 * (-oppSub  / towMain);
//            tempY2    -= tempZ2 * ( towSub  / towMain);
//            tempYOpp2 -= tempZ2 * (-oppSub  / towMain);
//            tempZ2    -= tempZ2 / 1;

//            tempX2 = Validation.checkZero(tempX2);
//            tempY2 = Validation.checkZero(tempY2);
//            tempZ2 = Validation.checkZero(tempZ2);
//            tempXOpp2 = Validation.checkZero(tempXOpp2);
//            tempYOpp2 = Validation.checkZero(tempYOpp2);
//            tempZOpp2 = Validation.checkZero(tempZOpp2);

//            if (abs(tempX2) <= UserVariables.accuracy &&
//                abs(tempY2) <= UserVariables.accuracy &&
//                abs(tempZ2) <= UserVariables.accuracy){
//                signalLogConsole("VHeights :" +
//                                 FS(tempX2) + " " + FS(tempXOpp2) + " " +
//                                 FS(tempY2) + " " + FS(tempYOpp2) + " " +
//                                 FS(tempZ2) + " " + FS(tempZOpp2));
//                signalLogConsole("Offs :" + FS(offsetX) + " " + FS(offsetY) + " " +FS(offsetZ));
//                signalLogConsole("No Hrad correction");

//                float smallest = qMin(offsetX, qMin(offsetY, offsetZ));

//                offsetX -= smallest;
//                offsetY -= smallest;
//                offsetZ -= smallest;

//                signalLogConsole("Offs :" +
//                                 QString::number(offsetX,'f',3) + " " +
//                                 QString::number(offsetY,'f',3) + " " +
//                                 QString::number(offsetZ,'f',3));

//                X    = tempX2;
//                XOpp = tempXOpp2;
//                Y    = tempY2;
//                YOpp = tempYOpp2;
//                Z    = tempZ2;
//                ZOpp = tempZOpp2;

//                //round to the nearest whole number
//                EEPROM.offsetX = int(offsetX+0.5);
//                EEPROM.offsetY = int(offsetY+0.5);
//                EEPROM.offsetZ = int(offsetZ+0.5);

//                j = 100;
//            }
//            else if (j == 99){
//                signalLogConsole("VHeights :" +
//                                 QString::number(tempX2,'f',3) + " " +
//                                 QString::number(tempXOpp2,'f',3) + " " +
//                                 QString::number(tempY2,'f',3) + " " +
//                                 QString::number(tempYOpp2,'f',3) + " " +
//                                 QString::number(tempZ2,'f',3) + " " +
//                                 QString::number(tempZOpp2,'f',3));
//                signalLogConsole("Offs :" +
//                                 offsetX + " " +
//                                 offsetY + " " +
//                                 offsetZ);
//                float dradCorr = tempX2 * -1.25F;
//                float HRadRatio = UserVariables.HRadRatio;

//                EEPROM.HRadius += dradCorr;

//                EEPROM.offsetX = 0;
//                EEPROM.offsetY = 0;
//                EEPROM.offsetZ = 0;

//                //hradsa = dradcorr
//                //solve inversely from previous method
//                float HRadOffset = HRadRatio * dradCorr;

//                tempX2    -= HRadOffset;
//                tempY2    -= HRadOffset;
//                tempZ2    -= HRadOffset;
//                tempXOpp2 -= HRadOffset;
//                tempYOpp2 -= HRadOffset;
//                tempZOpp2 -= HRadOffset;

//                signalLogConsole("Hrad correction: " + dradCorr);
//                signalLogConsole("HRad: " + EEPROM.HRadius.ToString());
//                j = 0;
//            }
//            else{
//                j++;
//            }

//            ////UserInterface.logConsole("Offs :" + offsetX + " " + offsetY + " " + offsetZ);
//            ////UserInterface.logConsole("VHeights :" + tempX2 + " " + tempXOpp2 + " " + tempY2 + " " + tempYOpp2 + " " + tempZ2 + " " + tempZOpp2);
//        }
//        else{
//            j = 100;
//            signalLogConsole("Tower Offsets and Delta Radii Calibrated");
//        }
//    }

//    if (EEPROM.offsetX > 1000 || EEPROM.offsetY > 1000 || EEPROM.offsetZ > 1000){
//        signalLogConsole("Tower offset calibration error, setting default values.");
//        signalLogConsole("Tower offsets before damage prevention: X" + offsetX + " Y" + offsetY + " Z" + offsetZ);
//        offsetX = 0;
//        offsetY = 0;
//        offsetZ = 0;
//    }
//    */
//}


//void CCalibration::alphaRotation(float& X,float& XOpp,float& Y,float& YOpp,float& Z,float& ZOpp)
//{
///*
//    float offsetX = EEPROM.offsetX;
//    float offsetY = EEPROM.offsetY;
//    float offsetZ = EEPROM.offsetZ;
//    float accuracy = UserVariables.accuracy;

//    //change to object
//    float alphaRotationPercentage = UserVariables.alphaRotationPercentage;

//    int k = 0;
//    while (k < 100){
//        //X Alpha Rotation
//        if (YOpp > ZOpp){
//            float ZYOppAvg = (YOpp - ZOpp) / 2;
//            EEPROM.A = EEPROM.A + (ZYOppAvg * alphaRotationPercentage); // (0.5/((diff y0 and z0 at X + 0.5)-(diff y0 and z0 at X = 0))) * 2 = 1.75
//            YOpp = YOpp - ZYOppAvg;
//            ZOpp = ZOpp + ZYOppAvg;
//        }
//        else if (YOpp < ZOpp){
//            float ZYOppAvg = (ZOpp - YOpp) / 2;

//            EEPROM.A = EEPROM.A - (ZYOppAvg * alphaRotationPercentage);
//            YOpp = YOpp + ZYOppAvg;
//            ZOpp = ZOpp - ZYOppAvg;
//        }

//        //Y Alpha Rotation
//        if (ZOpp > XOpp){
//            float XZOppAvg = (ZOpp - XOpp) / 2;
//            EEPROM.B = EEPROM.B + (XZOppAvg * alphaRotationPercentage);
//            ZOpp = ZOpp - XZOppAvg;
//            XOpp = XOpp + XZOppAvg;
//        }
//        else if (ZOpp < XOpp){
//            float XZOppAvg = (XOpp - ZOpp) / 2;

//            EEPROM.B = EEPROM.B - (XZOppAvg * alphaRotationPercentage);
//            ZOpp = ZOpp + XZOppAvg;
//            XOpp = XOpp - XZOppAvg;
//        }
//        //Z Alpha Rotation
//        if (XOpp > YOpp){
//            float YXOppAvg = (XOpp - YOpp) / 2;
//            EEPROM.C = EEPROM.C + (YXOppAvg * alphaRotationPercentage);
//            XOpp = XOpp - YXOppAvg;
//            YOpp = YOpp + YXOppAvg;
//        }
//        else if (XOpp < YOpp){
//            float YXOppAvg = (YOpp - XOpp) / 2;

//            EEPROM.C = EEPROM.C - (YXOppAvg * alphaRotationPercentage);
//            XOpp = XOpp + YXOppAvg;
//            YOpp = YOpp - YXOppAvg;
//        }

//        //determine if value is close enough
//        float hTow = qMax(qMax(XOpp, YOpp), ZOpp);
//        float lTow = qMin(qMin(XOpp, YOpp), ZOpp);
//        float towDiff = hTow - lTow;

//        if (towDiff <  UserVariables.calculationAccuracy &&
//            towDiff > -UserVariables.calculationAccuracy){
//            k = 100;

//            //log
//            signalLogConsole("ABC:" + EEPROM.A + " " + EEPROM.B + " " + EEPROM.C);
//        }
//        else{
//            k++;
//        }
//    }
//    */
//}

//void CCalibration::stepsPMM(float& X,float& XOpp,float& Y,float& YOpp,float& Z,float& ZOpp)
//{
///*
//    float diagChange = 1 / UserVariables.deltaOpp;
//    float towChange  = 1 / UserVariables.deltaTower;

//    float XYZ = (X + Y + Z) / 3;
//    float XYZOpp = (XOpp + YOpp + ZOpp) / 3;

//    EEPROM.stepsPerMM -= (XYZ - XYZOpp) * ((diagChange + towChange) / 2);

//    X += (XYZ - XYZOpp) * towChange;
//    Y += (XYZ - XYZOpp) * towChange;
//    Z += (XYZ - XYZOpp) * towChange;
//    XOpp += (XYZ - XYZOpp) * diagChange;
//    YOpp += (XYZ - XYZOpp) * diagChange;
//    ZOpp += (XYZ - XYZOpp) * diagChange;

//    signalLogConsole("Steps per Millimeter: " + EEPROM.stepsPerMM.ToString());
//    */
//}
