#include "dbct.h"
#include <math.h>
#include <QDebug>
#include "../../common.h"

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
CDbctHeight::CDbctHeight()
{
    X    =0;
    Y    =0;
    Z    =0;
    XOpp =0;
    YOpp =0;
    ZOpp =0;
}

QString CDbctHeight::toString()
{
    QString ret;
    ret  = QString("X=%1, "   ).arg(QString::number(X   ,'f',3));
    ret += QString("XOpp=%1, ").arg(QString::number(XOpp,'f',3));
    ret += QString("Y=%1, "   ).arg(QString::number(Y   ,'f',3));
    ret += QString("YOpp=%1, ").arg(QString::number(YOpp,'f',3));
    ret += QString("Z=%1, "   ).arg(QString::number(Z   ,'f',3));
    ret += QString("ZOpp=%1").arg(QString::number(ZOpp,'f',3));
    return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
CPrinterConfig::CPrinterConfig()
{
    probeHeight    = 0;
    stepPerMM      = 0;
    xEndstopOffset = 0;
    yEndstopOffset = 0;
    zEndstopOffset = 0;
    alphaA         = 0;
    alphaB         = 0;
    alphaC         = 0;
    deltaRadiusA   = 0;
    deltaRadiusB   = 0;
    deltaRadiusC   = 0;
}

QString CPrinterConfig::toString()
{
    QString ret;
    ret  = QString("Z-probe:                      %1\n").arg(QString::number(probeHeight,'f',3));
    ret += QString("X endstop offset:             %1\n").arg(QString::number(xEndstopOffset));
    ret += QString("Y endstop offset:             %1\n").arg(QString::number(yEndstopOffset));
    ret += QString("Z endstop offset:             %1\n").arg(QString::number(zEndstopOffset));
    ret += QString("Alpha A:                      %1\n").arg(QString::number(alphaA,'f',3));
    ret += QString("Alpha B:                      %1\n").arg(QString::number(alphaB,'f',3));
    ret += QString("Alpha C:                      %1\n").arg(QString::number(alphaC,'f',3));
    ret += QString("Delta Radius A:               %1\n").arg(QString::number(deltaRadiusA,'f',3));
    ret += QString("Delta Radius B:               %1\n").arg(QString::number(deltaRadiusB,'f',3));
    ret += QString("Delta Radius C:               %1\n").arg(QString::number(deltaRadiusC,'f',3));
    ret += QString("Step per mm:                  %1\n").arg(QString::number(stepPerMM,'f',3));
    ret += QString("Diagonal rod length:          %1\n").arg(QString::number(diagonalRodLength,'f',3));
    ret += QString("Horizontal rod radius at 0,0: %1\n").arg(QString::number(horizontalRodRadius,'f',3));

    return ret;
}

/*
CDbctCalibrate::CDbctCalibrate()
{
}

void CDbctCalibrate::Calibrate(Ui::dbct_wnd *parent)
{
    //Horizontal Radius Calibration*************************************************
    A    = parent->A->value();
    B    = parent->B->value();
    C    = parent->C->value();
    DA   = parent->DA->value();
    DB   = parent->DB->value();
    DC   = parent->DC->value();
    HRad = parent->HRad->value();
    measInver = parent->measInver->currentIndex();
    if (measInver == 2) measInver = -1;
    probeHeight = parent->probeHeight->value();

    X    = parent->X   ->value() * measInver - probeHeight;
    Y    = parent->Y   ->value() * measInver - probeHeight;
    Z    = parent->Z   ->value() * measInver - probeHeight;
    XOpp = parent->XOpp->value() * measInver - probeHeight;
    YOpp = parent->YOpp->value() * measInver - probeHeight;
    ZOpp = parent->ZOpp->value() * measInver - probeHeight;

    offsetX = parent->offsetX->value();
    offsetY = parent->offsetY->value();
    offsetZ = parent->offsetZ->value();
    offsetXYZ = 1 / 0.66;

    stepsPerMM = parent->stepsPerMM->value();

    //add one to hrad
//    var XTemp1 = parseFloat(document.getElementById('X1').value) * measInver - probeHeight;
    double XTemp1    = (X - HRad+1) * measInver - probeHeight;
//    var YTemp1 = parseFloat(document.getElementById('Y1').value) * measInver - probeHeight;
    double YTemp1    = (Y - HRad+1) * measInver - probeHeight;
//    var ZTemp1 = parseFloat(document.getElementById('Z1').value) * measInver - probeHeight;
    double ZTemp1    = (Z - HRad+1) * measInver - probeHeight;
//    var XOppTemp1 = parseFloat(document.getElementById('XOpp1').value) * measInver - probeHeight;
    double XOppTemp1 = (XOpp - HRad+1) * measInver - probeHeight;
//    var YOppTemp1 = parseFloat(document.getElementById('YOpp1').value) * measInver - probeHeight;
    double YOppTemp1 = (YOpp - HRad+1) * measInver - probeHeight;
//    var ZOppTemp1 = parseFloat(document.getElementById('ZOpp1').value) * measInver - probeHeight;
    double ZOppTemp1 = (ZOpp - HRad+1) * measInver - probeHeight;

    double HRadRatio;
    double newA;
    double newB;
    double newC;
    double newDA;
    double newDB;
    double newDC;
    double newHRad;

    accuracy = parent->accuracy->value();

    //Find the percentages for XYZ Offset
//    var xOppositePercentage = parseFloat(document.getElementById('xOppositePercentage').value);
    double xOppositePercentage = 0.5;
//    var yzPercentage = parseFloat(document.getElementById('yzPercentage').value);
    double yzPercentage = 0.25;
//    var X2 = parseFloat(document.getElementById('X2').value) * measInver - probeHeight;
    double X2    = (X - (double)offsetX/(double)stepsPerMM) * measInver - probeHeight;
//    var Y2 = parseFloat(document.getElementById('Y2').value) * measInver - probeHeight;
    double Y2    = (Y - (double)offsetY/(double)stepsPerMM) * measInver - probeHeight;
//    var Z2 = parseFloat(document.getElementById('Z2').value) * measInver - probeHeight;
    double Z2    = (Z - (double)offsetZ/(double)stepsPerMM) * measInver - probeHeight;
//    var XOpp2 = parseFloat(document.getElementById('XOpp2').value) * measInver - probeHeight;
    double XOpp2 = (XOpp - (double)offsetX/(double)stepsPerMM) * measInver - probeHeight;
//    var YOpp2 = parseFloat(document.getElementById('YOpp2').value) * measInver - probeHeight;
    double YOpp2 = (YOpp - (double)offsetX/(double)stepsPerMM) * measInver - probeHeight;
//    var ZOpp2 = parseFloat(document.getElementById('ZOpp2').value) * measInver - probeHeight;
    double ZOpp2 = (ZOpp - (double)offsetX/(double)stepsPerMM) * measInver - probeHeight;

//    var yOppositePercentage = parseFloat(document.getElementById('yOppositePercentage').value);
    double yOppositePercentage = 0.5;
//    var xzPercentage = parseFloat(document.getElementById('xzPercentage').value);
    double xzPercentage = 0.25;
//    var X3 = parseFloat(document.getElementById('X3').value) * measInver - probeHeight;
//    var Y3 = parseFloat(document.getElementById('Y3').value) * measInver - probeHeight;
//    var Z3 = parseFloat(document.getElementById('Z3').value) * measInver - probeHeight;
//    var XOpp3 = parseFloat(document.getElementById('XOpp3').value) * measInver - probeHeight;
//    var YOpp3 = parseFloat(document.getElementById('YOpp3').value) * measInver - probeHeight;
//    var ZOpp3 = parseFloat(document.getElementById('ZOpp3').value) * measInver - probeHeight;

//    var zOppositePercentage = parseFloat(document.getElementById('zOppositePercentage').value);
    double zOppositePercentage = 0.5;
//    var xyPercentage = parseFloat(document.getElementById('xyPercentage').value);
    double zOppositePercentage = 0.25;
//    var X4 = parseFloat(document.getElementById('X4').value) * measInver - probeHeight;
//    var Y4 = parseFloat(document.getElementById('Y4').value) * measInver - probeHeight;
//    var Z4 = parseFloat(document.getElementById('Z4').value) * measInver - probeHeight;
//    var XOpp4 = parseFloat(document.getElementById('XOpp4').value) * measInver - probeHeight;
//    var YOpp4 = parseFloat(document.getElementById('YOpp4').value) * measInver - probeHeight;
//    var ZOpp4 = parseFloat(document.getElementById('ZOpp4').value) * measInver - probeHeight;

    double xOppPerc;
    double yzPerc;

    double yOppPerc;
    double xzPerc;

    double zOppPerc;
    double xyPerc;


    if (xOppositePercentage > 0) {
        var xOppPerc = xOppositePercentage;
        var yzPerc = yzPercentage;

        var yOppPerc = yOppositePercentage;
        var xzPerc = xzPercentage;

        var zOppPerc = zOppositePercentage;
        var xyPerc = xyPercentage;
    } else {
        var xOppPerc = getPercentagesOpp("X", X, Y, Z, XOpp, YOpp, ZOpp, X2, Y2, Z2, XOpp2, YOpp2, ZOpp2);
        var yzPerc = getPercentagesAll("X", X, Y, Z, XOpp, YOpp, ZOpp, X2, Y2, Z2, XOpp2, YOpp2, ZOpp2);

        var yOppPerc = getPercentagesOpp("Y", X, Y, Z, XOpp, YOpp, ZOpp, X3, Y3, Z3, XOpp3, YOpp3, ZOpp3);
        var xzPerc = getPercentagesAll("Y", X, Y, Z, XOpp, YOpp, ZOpp, X3, Y3, Z3, XOpp3, YOpp3, ZOpp3);

        var zOppPerc = getPercentagesOpp("Z", X, Y, Z, XOpp, YOpp, ZOpp, X4, Y4, Z4, XOpp4, YOpp4, ZOpp4);
        var xyPerc = getPercentagesAll("Z", X, Y, Z, XOpp, YOpp, ZOpp, X4, Y4, Z4, XOpp4, YOpp4, ZOpp4);

        console.log("Tower Offset Percentages: ", xOppPerc, yzPerc, yOppPerc, xzPerc, zOppPerc, xyPerc);

        document.getElementById('xOppositePercentage').value = xOppPerc;
        document.getElementById('yzPercentage').value = yzPerc;
        document.getElementById('yOppositePercentage').value = yOppPerc;
        document.getElementById('xzPercentage').value = xzPerc;
        document.getElementById('zOppositePercentage').value = yOppPerc;
        document.getElementById('xyPercentage').value = xyPerc;
    }

    //find the percentages for diagonal rod offset
    var XTemp5 = parseFloat(document.getElementById('X5').value) * measInver;
    var YTemp5 = parseFloat(document.getElementById('Y5').value) * measInver;
    var ZTemp5 = parseFloat(document.getElementById('Z5').value) * measInver;
    var XOppTemp5 = parseFloat(document.getElementById('XOpp5').value) * measInver;
    var YOppTemp5 = parseFloat(document.getElementById('YOpp5').value) * measInver;
    var ZOppTemp5 = parseFloat(document.getElementById('ZOpp5').value) * measInver;

    var delTower = parseFloat(document.getElementById('deltaTower').value);
    var delOpp = parseFloat(document.getElementById('deltaOpp').value);
    var diagonalRod = parseFloat(document.getElementById('diagonalRod').value);

    if (delTower > 0 && delOpp > 0) {
        var deltaTower = delTower;
        var deltaOpp = delOpp;
    } else {
        var deltaTower = ((X - XTemp5) + (Y - YTemp5) + (Z - ZTemp5)) / 3;
        var deltaOpp = ((XOpp - XOppTemp5) + (YOpp - YOppTemp5) + (ZOpp - ZOppTemp5)) / 3;

        document.getElementById('deltaTower').value = deltaTower; // 1/8
        document.getElementById('deltaOpp').value = deltaOpp; // 1/4
    }

    deltaTower = Math.abs(deltaTower);
    deltaOpp = Math.abs(deltaOpp);


//HRad is calibrated by increasing the outside edge of the glass by the average differences, this should balance the values with a central point of around zero
    //calculate HRad Ratio
    var HRadManual = parseFloat(document.getElementById('HRadManual').value);
    if (HRadManual > 0 || HRadManual < 0) {
        HRadRatio = HRadManual;
        console.log("Hrad Ratio: ", HRadRatio);
    } else {
        HRadRatio = -(Math.abs((XTemp1 - X) + (YTemp1 - Y) + (ZTemp1 - Z) + (XOppTemp1 - XOpp) + (YOppTemp1 - YOpp) + (ZOppTemp1 - ZOpp))) / 6;
        HRadRatio = HRadRatio.toFixed(4);
        console.log("Hrad Ratio: ", HRadRatio);
        document.getElementById('HRadManual').value = HRadRatio;
    }
    var HRadSA = ((X + XOpp + Y + YOpp + Z + ZOpp) / 6);
    HRadSA = parseFloat(HRadSA.toFixed(4));
    HRad = HRad + (HRadSA / HRadRatio);
    console.log("HRad change in mm: ", HRadSA);
    X = X - HRadSA;
    Y = Y - HRadSA;
    Z = Z - HRadSA;
    XOpp = XOpp - HRadSA;
    YOpp = YOpp - HRadSA;
    ZOpp = ZOpp - HRadSA;
    //check if values are close to zero, then set them to zero - avoids errors
    X = checkZero(X);
    Y = checkZero(Y);
    Z = checkZero(Z);
    XOpp = checkZero(XOpp);
    YOpp = checkZero(YOpp);
    ZOpp = checkZero(ZOpp);
    //modify XYZ,O values to match the new horizontal radius

    console.log("Tower heights after horizontal radius calibration: ", X, XOpp, Y, YOpp, Z, ZOpp);


    //checks the calibration of the horizontal radius
    calibVerify();

    var DASA = ((X + XOpp) / 2);
    var DBSA = ((Y + YOpp) / 2);
    var DCSA = ((Z + ZOpp) / 2);
    DA = DA + ((DASA) / HRadRatio);
    DB = DB + ((DBSA) / HRadRatio);
    DC = DC + ((DCSA) / HRadRatio);

    if (DA > 1 || DA < -1) {
        document.getElementById('DA').value = DA.toPrecision(4);
    } else if (DA > 0 && DA < 0.001) {
        document.getElementById('DA').value = 0;
        DA = 0;
    } else if (DA < 0 && DA > -0.001) {
        document.getElementById('DA').value = 0;
        DA = 0;
    } else {
        document.getElementById('DA').value = DA.toPrecision(3);
    }
    if (DB > 1 || DB < -1) {
        document.getElementById('DB').value = DB.toPrecision(4);
    } else if (DB > 0 && DB < 0.001) {
        document.getElementById('DB').value = 0;
        DB = 0;
    } else if (DB < 0 && DB > -0.001) {
        document.getElementById('DB').value = 0;
        DB = 0;
    } else {
        document.getElementById('DB').value = DB.toPrecision(3);
    }
    if (DC > 1 || DC < -1) {
        document.getElementById('DC').value = DC.toPrecision(4);
    } else if (DC > 0 && DC < 0.001) {
        document.getElementById('DC').value = 0;
        DC = 0;
    } else if (DC < 0 && DC > -0.001) {
        document.getElementById('DC').value = 0;
        DC = 0;
    } else {
        document.getElementById('DC').value = DC.toPrecision(3);
    }
    ////////////////////////////////////////////////////////////////////////////////
    //Tower Offset Calibration******************************************************
    var getSteps = document.getElementById('stepsPerMM').value;
    if (getSteps > 0) {
        var stepsPerMM = getSteps;
    } else {
        var motorStepAngle = document.getElementById('stepSize').value;
        var driverMicroStepping = document.getElementById('microStepping').value;
        var beltPitch = document.getElementById('beltPitch').value;
        var toothCount = document.getElementById('toothCount').value;
        stepsPerMM = ((360 / motorStepAngle) * driverMicroStepping)/(beltPitch * toothCount);
        document.getElementById('stepsPerMM').value = stepsPerMM;
    }
    //balance axes
    var offsetX = parseFloat(document.getElementById('offsetX').value);
    var offsetY = parseFloat(document.getElementById('offsetY').value);
    var offsetZ = parseFloat(document.getElementById('offsetZ').value);
    var offsetXYZ = 1 / 0.66;

    //XYZ offset
    //X
    var xxOppPerc = 0.5;
    var xyPerc = 0.25;
    var xyOppPerc = 0.25;
    var xzPerc = 0.25;
    var xzOppPerc = 0.25;

    //Y
    var yyOppPerc = 0.5;
    var yxPerc = 0.25;
    var yxOppPerc = 0.25;
    var yzPerc = 0.25;
    var yzOppPerc = 0.25;

    //Z
    var zzOppPerc = 0.5;
    var zxPerc = 0.25;
    var zxOppPerc = 0.25;
    var zyPerc = 0.25;
    var zyOppPerc = 0.25;

    var j = 0;
    //correction of one tower allows the diagonal rod to respond more accurately
    while (j < 1)
    {
        var theoryX = offsetX + X * stepsPerMM * offsetXYZ;

        //correction of one tower allows for XY dimensional accuracy
        if (X > 0)
        {
            //if x is positive
            offsetX = offsetX + X * stepsPerMM * offsetXYZ;

            XOpp = XOpp + (X * xxOppPerc);//0.5
            Z = Z + (X * xzPerc);//0.25
            Y = Y + (X * xyPerc);//0.25
            ZOpp = ZOpp - (X * xzOppPerc);//0.25
            YOpp = YOpp - (X * xyOppPerc);//0.25
            X = X - X;
        }
        else if (theoryX > 0 && X < 0)
        {
            //if x is negative and can be decreased
            offsetX = offsetX + X * stepsPerMM * offsetXYZ;

            XOpp = XOpp + (X * xxOppPerc);//0.5
            Z = Z + (X * xzPerc);//0.25
            Y = Y + (X * xyPerc);//0.25
            ZOpp = ZOpp - (X * xzOppPerc);//0.25
            YOpp = YOpp - (X * xyOppPerc);//0.25
            X = X - X;
        }
        else
        {
            //if X is negative
            offsetY = offsetY - X * stepsPerMM * offsetXYZ * 2;
            offsetZ = offsetZ - X * stepsPerMM * offsetXYZ * 2;

            YOpp = YOpp - (X * 2 * yyOppPerc);
            X = X - (X * 2 * yxPerc);
            Z = Z - (X * 2 * yxPerc);
            XOpp = XOpp + (X * 2 * yxOppPerc);
            ZOpp = ZOpp + (X * 2 * yxOppPerc);
            Y = Y + X * 2;

            ZOpp = ZOpp - (X * 2 * zzOppPerc);
            X = X - (X * 2 * zxPerc);
            Y = Y - (X * 2 * zyPerc);
            XOpp = XOpp + (X * 2 * yxOppPerc);
            YOpp = YOpp + (X * 2 * zyOppPerc);
            Z = Z + X * 2;
        }

        var theoryY = offsetY + Y * stepsPerMM * offsetXYZ;

        //Y
        if (Y > 0)
        {
            offsetY = offsetY + Y * stepsPerMM * offsetXYZ;

            YOpp = YOpp + (Y * yyOppPerc);
            X = X + (Y * yxPerc);
            Z = Z + (Y * yxPerc);
            XOpp = XOpp - (Y * yxOppPerc);
            ZOpp = ZOpp - (Y * yxOppPerc);
            Y = Y - Y;
        }
        else if (theoryY > 0 && Y < 0)
        {
            offsetY = offsetY + Y * stepsPerMM * offsetXYZ;

            YOpp = YOpp + (Y * yyOppPerc);
            X = X + (Y * yxPerc);
            Z = Z + (Y * yxPerc);
            XOpp = XOpp - (Y * yxOppPerc);
            ZOpp = ZOpp - (Y * yxOppPerc);
            Y = Y - Y;
        }
        else
        {
            offsetX = offsetX - Y * stepsPerMM * offsetXYZ * 2;
            offsetZ = offsetZ - Y * stepsPerMM * offsetXYZ * 2;

            XOpp = XOpp - (Y * 2 * xxOppPerc);//0.5
            Z = Z - (Y * 2 * xzPerc);//0.25
            Y = Y - (Y * 2 * xyPerc);//0.25
            ZOpp = ZOpp + (Y * 2 * xzOppPerc);//0.25
            YOpp = YOpp + (Y * 2 * xyOppPerc);//0.25
            X = X + Y * 2;

            ZOpp = ZOpp - (Y * 2 * zzOppPerc);
            X = X - (Y * 2 * zxPerc);
            Y = Y - (Y * 2 * zyPerc);
            XOpp = XOpp + (Y * 2 * yxOppPerc);
            YOpp = YOpp + (Y * 2 * zyOppPerc);
            Z = Z + Y * 2;
        }

        var theoryZ = offsetZ + Z * stepsPerMM * offsetXYZ;

        //Z
        if (Z > 0)
        {
            offsetZ = offsetZ + Z * stepsPerMM * offsetXYZ;

            ZOpp = ZOpp + (Z * zzOppPerc);
            X = X + (Z * zxPerc);
            Y = Y + (Z * zyPerc);
            XOpp = XOpp - (Z * yxOppPerc);
            YOpp = YOpp - (Z * zyOppPerc);
            Z = Z - Z;
        }
        else if (theoryZ > 0 && Z < 0)
        {
            offsetZ = offsetZ + Z * stepsPerMM * offsetXYZ;

            ZOpp = ZOpp + (Z * zzOppPerc);
            X = X + (Z * zxPerc);
            Y = Y + (Z * zyPerc);
            XOpp = XOpp - (Z * yxOppPerc);
            YOpp = YOpp - (Z * zyOppPerc);
            Z = Z - Z;
        }
        else
        {
            offsetY = offsetY - Z * stepsPerMM * offsetXYZ * 2;
            offsetX = offsetX - Z * stepsPerMM * offsetXYZ * 2;

            XOpp = XOpp - (Z * 2 * xxOppPerc);//0.5
            Z = Z - (Z * 2 * xzPerc);//0.25
            Y = Y - (Z * 2 * xyPerc);//0.25
            ZOpp = ZOpp + (Z * 2 * xzOppPerc);//0.25
            YOpp = YOpp + (Z * 2 * xyOppPerc);//0.25
            X = X + Z * 2;

            YOpp = YOpp - (Z * 2 * yyOppPerc);
            X = X - (Z * 2 * yxPerc);
            Z = Z - (Z * 2 * yxPerc);
            XOpp = XOpp + (Z * 2 * yxOppPerc);
            ZOpp = ZOpp + (Z * 2 * yxOppPerc);
            Y = Y + Z * 2;
        }

        X = checkZero(X);
        Y = checkZero(Y);
        Z = checkZero(Z);
        XOpp = checkZero(XOpp);
        YOpp = checkZero(YOpp);
        ZOpp = checkZero(ZOpp);

        console.log("Virtual heights: X:", X, ", XOpp:", XOpp, ", Y:", Y, ", YOpp:", YOpp, ", Z:", Z, ", and ZOpp:", ZOpp, "\n");

        var accuracy = 0.005;
        if (X < accuracy && X > -accuracy && Y < accuracy && Y > -accuracy && Z < accuracy && Z > -accuracy)
        {
            j = 1;
        }
    }

    document.getElementById('offsetX').value = Math.round(offsetX);
    document.getElementById('offsetY').value = Math.round(offsetY);
    document.getElementById('offsetZ').value = Math.round(offsetZ);


    //Alpha Rotation Calibration****************************************************
    //Iterate through until balanced
    //change loop to be based on difference between XOpp, YOpp, ZOpp
    for (var i = 0; i < 1;) {
        //X Alpha Rotation
        if (YOpp > ZOpp) {
            var ZYOppAvg = (YOpp - ZOpp) / 2;
            A = A + (ZYOppAvg * 1.725); // (0.5/((diff y0 and z0 at X + 0.5)-(diff y0 and z0 at X = 0))) * 2 = 1.75
            YOpp = YOpp - ZYOppAvg;
            ZOpp = ZOpp + ZYOppAvg;
        } else if (YOpp < ZOpp) {
            var ZYOppAvg = (ZOpp - YOpp) / 2;

            A = A - (ZYOppAvg * 1.725);
            YOpp = YOpp + ZYOppAvg;
            ZOpp = ZOpp - ZYOppAvg;
        }

        //Y Alpha Rotation
        if (ZOpp > XOpp) {
            var XZOppAvg = (ZOpp - XOpp) / 2;
            B = B + (XZOppAvg * 1.725);
            ZOpp = ZOpp - XZOppAvg;
            XOpp = XOpp + XZOppAvg;
        } else if (ZOpp < XOpp) {
            var XZOppAvg = (XOpp - ZOpp) / 2;

            B = B - (XZOppAvg * 1.725);
            ZOpp = ZOpp + XZOppAvg;
            XOpp = XOpp - XZOppAvg;
        }
        //Z Alpha Rotation
        if (XOpp > YOpp) {
            var YXOppAvg = (XOpp - YOpp) / 2;
            C = C + (YXOppAvg * 1.725);
            XOpp = XOpp - YXOppAvg;
            YOpp = YOpp + YXOppAvg;
        } else if (XOpp < YOpp) {
            var YXOppAvg = (YOpp - XOpp) / 2;

            C = C - (YXOppAvg * 1.725);
            XOpp = XOpp + YXOppAvg;
            YOpp = YOpp - YXOppAvg;
        }
        //determine if value is close enough
        var hTow = Math.max(XOpp, YOpp, ZOpp);
        var lTow = Math.min(XOpp, YOpp, ZOpp);
        var towDiff = hTow - lTow;
        towDiff = parseFloat(towDiff.toFixed(6));
        if (towDiff < 0.0001) {
            i = 1;
            console.log("Alpha Rotation Calibrated to: ", towDiff);
        }
    }
    //Diagonal Rod Calibration******************************************************
    var XTemp5 = parseFloat(document.getElementById('X5').value);
    var YTemp5 = parseFloat(document.getElementById('Y5').value);
    var ZTemp5 = parseFloat(document.getElementById('Z5').value);
    var XOppTemp5 = parseFloat(document.getElementById('XOpp5').value);
    var YOppTemp5 = parseFloat(document.getElementById('YOpp5').value);
    var ZOppTemp5 = parseFloat(document.getElementById('ZOpp5').value);
    //var deltaTower = 0.13083;
    //var deltaOpp = 0.21083;
    var delTower = parseFloat(document.getElementById('deltaTower').value);
    var delOpp = parseFloat(document.getElementById('deltaOpp').value);
    var diagonalRod = parseFloat(document.getElementById('diagonalRod').value);
    if (delTower > 0 && delOpp > 0) {
        var deltaTower = delTower;
        var deltaOpp = delOpp;
    } else {
        var deltaTower = ((X - XTemp5) + (Y - YTemp5) + (Z - ZTemp5)) / 3;
        var deltaOpp = ((XOpp - XOppTemp5) + (YOpp - YOppTemp5) + (ZOpp - ZOppTemp5)) / 3;
        document.getElementById('deltaTower').value = deltaTower; // 1/8
        document.getElementById('deltaOpp').value = deltaOpp; // 1/4
    }
    deltaTower = Math.abs(deltaTower);
    deltaOpp = Math.abs(deltaOpp);
    var diagChange = 1 / deltaOpp;
    var towOppDiff = deltaTower / deltaOpp; //0.5
    //increase decreases all towers
    //decrease increases all towers
    for (var i = 0; i < 1;) {
        var XYZOpp = (XOpp + YOpp + ZOpp) / 3;
        diagonalRod = diagonalRod + (XYZOpp * diagChange);
        X = X - towOppDiff * XYZOpp;
        Y = Y - towOppDiff * XYZOpp;
        Z = Z - towOppDiff * XYZOpp;
        XOpp = XOpp - XYZOpp;
        YOpp = YOpp - XYZOpp;
        ZOpp = ZOpp - XYZOpp;
        XYZOpp = (XOpp + YOpp + ZOpp) / 3;
        XYZOpp = checkZero(XYZOpp);

        var XYZ = (X + Y + Z) / 3;
        //hrad
        HRad = HRad + (XYZ / HRadRatio);

        if (XYZOpp >= 0) {
            X = X - XYZ;
            Y = Y - XYZ;
            Z = Z - XYZ;
            XOpp = XOpp - XYZ;
            YOpp = YOpp - XYZ;
            ZOpp = ZOpp - XYZ;
        } else {
            X = X + XYZ;
            Y = Y + XYZ;
            Z = Z + XYZ;
            XOpp = XOpp + XYZ;
            YOpp = YOpp + XYZ;
            ZOpp = ZOpp + XYZ;
        }
        //XYZ is zero
        if (XYZOpp < 0.0001 && XYZOpp > -0.0001 && XYZ < 0.0001 && XYZ > -0.0001) {
            i = 1;
            diagonalRod = parseFloat(diagonalRod.toFixed(6));
            console.log("Diagonal Rod Calibrated to: ", diagonalRod);
            document.getElementById('diagonalRod').value = diagonalRod.toFixed(3);
        }
    }
    //send obtained values back to the document*************************************
    document.getElementById('A').value = A.toFixed(3);
    document.getElementById('B').value = B.toFixed(3);
    document.getElementById('C').value = C.toFixed(3);

    document.getElementById('HRad').value = HRad.toFixed(3);
    document.getElementById('HRadManual').value = HRadRatio;
    console.log("New tower heights: ", checkZero(X), checkZero(XOpp), checkZero(Y), checkZero(YOpp), checkZero(Z), checkZero(ZOpp));
    console.log("New horizontal radius: ", HRad);
    console.log("New delta radii: ", DA, DB, DC);
    console.log("New alpha rotation: ", A, B, C);
}

double CDbctCalibrate::getPercentagesOpp(QChar _XYZ,double _X    ,double _Y    ,double _Z,
                                                    double _XOpp ,double _YOpp ,double _ZOpp,
                                                    double _X2   ,double _Y2   ,double _Z2,
                                                    double _XOpp2,double _YOpp2,double _ZOpp2)
{
    double percentages = 0.0;
    double xDiff    = abs(_X2    -    _X);
    double yDiff    = abs(_Y2    -    _Y);
    double zDiff    = abs(_Z2    -    _Z);
    double xOppDiff = abs(_XOpp2 - _XOpp);
    double yOppDiff = abs(_YOpp2 - _YOpp);
    double zOppDiff = abs(_ZOpp2 - _ZOpp);
    if      (_XYZ == 'X')
        percentages = xOppDiff / xDiff;
    else if (_XYZ == 'Y')
        percentages = yOppDiff / yDiff;
    else if (_XYZ == 'Z')
        percentages = zOppDiff / zDiff;
//    percentages = percentages.toFixed(3);
    return percentages;
}

double CDbctCalibrate::getPercentagesOpp(QChar _XYZ,double _X    ,double _Y    ,double _Z,
                                                    double _XOpp ,double _YOpp ,double _ZOpp,
                                                    double _X2   ,double _Y2   ,double _Z2,
                                                    double _XOpp2,double _YOpp2,double _ZOpp2)
{
    double percentages = 0.0;
    double xDiff    = abs(_X2    -    _X);
    double yDiff    = abs(_Y2    -    _Y);
    double zDiff    = abs(_Z2    -    _Z);
    double xOppDiff = abs(_XOpp2 - _XOpp);
    double yOppDiff = abs(_YOpp2 - _YOpp);
    double zOppDiff = abs(_ZOpp2 - _ZOpp);
    if      (_XYZ == 'X')
        percentages = (yDiff + zDiff + yOppDiff + zOppDiff) / (xDiff * 4);
    else if (_XYZ == 'Y')
        percentages = (xDiff + zDiff + xOppDiff + zOppDiff) / (yDiff * 4);
    else if (_XYZ == 'Z')
        percentages = (yDiff + xDiff + yOppDiff + xOppDiff) / (zDiff * 4);
//    percentages = percentages.toFixed(3);
    return percentages;
}

void CDbctCalibrate::calibVerify()
{
    //checks if the value obtained by the new HRad is correct
    if (HRadSA < 0.1) {
        if (HRadSA < 0.0001) {
            HRadSA = 0;
        }
        console.append(QString("Horizontal Radius Calibration Success - %s").arg(QString::number(HRadSA,'f',4)));
    }
    else {
        console.append(QString("Horizontal Radius Calibration Failure - %s").arg(QString::number(HRadSA,'f',4)));
    }
}

double CDbctCalibrate::checkZero(double value)
{
    if (value > -0.0001 && value < 0.0001) return 0;
    else return value;
}

//function calibrate()
//{
    //Horizontal Radius Calibration*************************************************
//    var A = parseFloat(document.getElementById('A').value);
//    var B = parseFloat(document.getElementById('B').value);
//    var C = parseFloat(document.getElementById('C').value);
//    var DA = parseFloat(document.getElementById('DA').value);
//    var DB = parseFloat(document.getElementById('DB').value);
//    var DC = parseFloat(document.getElementById('DC').value);
//    var HRad = parseFloat(document.getElementById('HRad').value);
//    var measInver = parseFloat(document.getElementById('measInver').value);
//    var probeHeight = parseFloat(document.getElementById('probeHeight').value);

//    var X = parseFloat(document.getElementById('X').value) * measInver - probeHeight;
//    var Y = parseFloat(document.getElementById('Y').value) * measInver - probeHeight;
//    var Z = parseFloat(document.getElementById('Z').value) * measInver - probeHeight;
//    var XOpp = parseFloat(document.getElementById('XOpp').value) * measInver - probeHeight;
//    var YOpp = parseFloat(document.getElementById('YOpp').value) * measInver - probeHeight;
//    var ZOpp = parseFloat(document.getElementById('ZOpp').value) * measInver - probeHeight;

//    //add one to hrad
//    var XTemp1 = parseFloat(document.getElementById('X1').value) * measInver - probeHeight;
//    var YTemp1 = parseFloat(document.getElementById('Y1').value) * measInver - probeHeight;
//    var ZTemp1 = parseFloat(document.getElementById('Z1').value) * measInver - probeHeight;
//    var XOppTemp1 = parseFloat(document.getElementById('XOpp1').value) * measInver - probeHeight;
//    var YOppTemp1 = parseFloat(document.getElementById('YOpp1').value) * measInver - probeHeight;
//    var ZOppTemp1 = parseFloat(document.getElementById('ZOpp1').value) * measInver - probeHeight;


//    var HRadRatio;
//    var newA;
//    var newB;
//    var newC;
//    var newDA;
//    var newDB;
//    var newDC;
//    var newHRad;

//    var accuracy = parseFloat(document.getElementById('accuracy').value);

//    //Find the percentages for XYZ Offset
//    var xOppositePercentage = parseFloat(document.getElementById('xOppositePercentage').value);
//    var yzPercentage = parseFloat(document.getElementById('yzPercentage').value);
//    var X2 = parseFloat(document.getElementById('X2').value) * measInver - probeHeight;
//    var Y2 = parseFloat(document.getElementById('Y2').value) * measInver - probeHeight;
//    var Z2 = parseFloat(document.getElementById('Z2').value) * measInver - probeHeight;
//    var XOpp2 = parseFloat(document.getElementById('XOpp2').value) * measInver - probeHeight;
//    var YOpp2 = parseFloat(document.getElementById('YOpp2').value) * measInver - probeHeight;
//    var ZOpp2 = parseFloat(document.getElementById('ZOpp2').value) * measInver - probeHeight;

//    var yOppositePercentage = parseFloat(document.getElementById('yOppositePercentage').value);
//    var xzPercentage = parseFloat(document.getElementById('xzPercentage').value);
//    var X3 = parseFloat(document.getElementById('X3').value) * measInver - probeHeight;
//    var Y3 = parseFloat(document.getElementById('Y3').value) * measInver - probeHeight;
//    var Z3 = parseFloat(document.getElementById('Z3').value) * measInver - probeHeight;
//    var XOpp3 = parseFloat(document.getElementById('XOpp3').value) * measInver - probeHeight;
//    var YOpp3 = parseFloat(document.getElementById('YOpp3').value) * measInver - probeHeight;
//    var ZOpp3 = parseFloat(document.getElementById('ZOpp3').value) * measInver - probeHeight;

//    var zOppositePercentage = parseFloat(document.getElementById('zOppositePercentage').value);
//    var xyPercentage = parseFloat(document.getElementById('xyPercentage').value);
//    var X4 = parseFloat(document.getElementById('X4').value) * measInver - probeHeight;
//    var Y4 = parseFloat(document.getElementById('Y4').value) * measInver - probeHeight;
//    var Z4 = parseFloat(document.getElementById('Z4').value) * measInver - probeHeight;
//    var XOpp4 = parseFloat(document.getElementById('XOpp4').value) * measInver - probeHeight;
//    var YOpp4 = parseFloat(document.getElementById('YOpp4').value) * measInver - probeHeight;
//    var ZOpp4 = parseFloat(document.getElementById('ZOpp4').value) * measInver - probeHeight;

*/
