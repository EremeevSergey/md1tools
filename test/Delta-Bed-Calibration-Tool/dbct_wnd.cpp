#include "dbct_wnd.h"
#include "../../common.h"
#include <math.h>
#include <QApplication>

#define TEST_PLANE_RADIUS 70
#define TEST_PROBE_START_OFFSET 30

void CDeltaBedCalibrationTools::mainLoop()
{
    switch (state) {
    case Active:
        activeLoop();
        break;
    case WaitStop:
        state = Stoped;
        stop();
        break;
    default:
        break;
    }
    updateControls();
}

void CDeltaBedCalibrationTools::activeLoop    ()
{
    double z;
    int index;
    CDbctHeight*  height;
    QStringList   script;
    CEePromRecord record;
    switch (step){
    case ReadEEPROM:
        // Считили ЕЕПРОМ. Сохраняем начальные условия
        readEeprom();
        currentHeightIndex=0;
        Printer.sendGoHomeAll();
        teLog->appendPlainText(PrinterConfig.toString());
        step = GoHome;
        break;
    case GoHome:
        // Пришли домой теперь надо промерить вершины
        currentVertexIndex=0;
        gotoxyz();
        break;
    case GoToXYZ:
        // Измеряем расстояние до стола
        Printer.sendGetZProbeValue();
        step = ZProbe;
        break;
    case ExecuteCommand:
        Printer.sendGoHomeAll();
        step = GoHome;
        break;
    case Process:
        calculate();
        step = GoHome;
        break;
    case ZProbe:
        z = Printer.ZProbe.Z - TestProbeStartOffset;
        height = Height+currentHeightIndex;
        if      (currentVertexIndex==0)  height->X    = z;
        else if (currentVertexIndex==1)  height->XOpp = z;
        else if (currentVertexIndex==2)  height->Y    = z;
        else if (currentVertexIndex==3)  height->YOpp = z;
        else if (currentVertexIndex==4)  height->Z    = z;
        else if (currentVertexIndex==5)  height->ZOpp = z;
        currentVertexIndex++;
        if (!gotoxyz()){
            // Промерили все вершины
            currentHeightIndex++;
            teLog->appendPlainText(height->toString());
            if (currentHeightIndex>5){
                // промерили всё
                // пока стоп
                Printer.sendScript(PrepareScript());
                step = Process;
            }
            else{
                // Надо произвести изменения в текущих настройках
                // А пока измеряем ещё раз
                script = PrepareScript();
                if      (currentHeightIndex==1){
                // Increase your printers Horizontal radius by 1 unit (change nothing else)
                // and write the XYZ and Opposing tower heights here to calculate the increment.
                    index = Printer.EEPROM->indexByStartName("Horizontal rod radius at");
                    if (index>=0){
                        record=CEePromRecord(Printer.EEPROM->at(index));
                        record.FValue+=1.0;
                        script.append(record.ToCmdString());
                        teLog->appendPlainText(QString("Increase printers Horizontal radius by 1 unit (%1 -> %2)").
                                               arg(QString::number(record.FValue-1.0,'f',3)).
                                               arg(QString::number(record.FValue,'f',3)));
                    }
                }
                else if (currentHeightIndex==2){
                    // Increase your printers X Tower Offset by 80 steps (change nothing else)
                    // and write the XYZ and Opposing tower heights here to calculate the change manually.
                    index = Printer.EEPROM->indexByStartName("Tower X endstop offset");
                    if (index>=0){
                        record=CEePromRecord(Printer.EEPROM->at(index));
                        record.IValue+=PrinterConfig.stepPerMM;
                        script.append(record.ToCmdString());
                        teLog->appendPlainText(QString("Increase printers Tower X endstop offset by step_per_mm (%1 -> %2)").
                                               arg(QString::number(record.IValue-PrinterConfig.stepPerMM,'f',3)).
                                               arg(QString::number(record.IValue,'f',3)));
                    }
                }
                else if (currentHeightIndex==3){
                    // Increase your printers Y Tower Offset by 80 steps (change nothing else)
                    // and write the XYZ and Opposing tower heights here to calculate the change manually.
                    index = Printer.EEPROM->indexByStartName("Tower Y endstop offset");
                    if (index>=0){
                        record=CEePromRecord(Printer.EEPROM->at(index));
                        record.IValue+=PrinterConfig.stepPerMM;
                        script.append(record.ToCmdString());
                        teLog->appendPlainText(QString("Increase printers Tower Y endstop offset by step_per_mm (%1 -> %2)").
                                               arg(QString::number(record.IValue-PrinterConfig.stepPerMM,'f',3)).
                                               arg(QString::number(record.IValue,'f',3)));
                    }
                }
                else if (currentHeightIndex==4){
                    // Increase your printers Z Tower Offset by 80 steps (change nothing else)
                    // and write the XYZ and Opposing tower heights here to calculate the change manually.
                    index = Printer.EEPROM->indexByStartName("Tower Z endstop offset");
                    if (index>=0){
                        record=CEePromRecord(Printer.EEPROM->at(index));
                        record.IValue+=PrinterConfig.stepPerMM;
                        script.append(record.ToCmdString());
                        teLog->appendPlainText(QString("Increase printers Tower Z endstop offset by step_per_mm (%1 -> %2)").
                                               arg(QString::number(record.IValue-PrinterConfig.stepPerMM,'f',3)).
                                               arg(QString::number(record.IValue,'f',3)));
                    }
                }
                else if (currentHeightIndex==5){
                    // Increase your printers diagonal rod length by 1 (change nothing else) unit and write
                    // the XYZ and Opposing tower heights here to calculate how far off the rod length is.
                    index = Printer.EEPROM->indexByStartName("Diagonal rod length");
                    if (index>=0){
                        record=CEePromRecord(Printer.EEPROM->at(index));
                        record.FValue+=1.0;
                        script.append(record.ToCmdString());
                        teLog->appendPlainText(QString("Increase printers diagonal rod length by 1 (%1 -> %2)").
                                               arg(QString::number(record.FValue-1.0,'f',3)).
                                               arg(QString::number(record.FValue,'f',3)));
                    }
                }
                Printer.sendScript(script);
                step = ExecuteCommand;
            }
        }
        break;
    default:
        break;
    }

}

void CDeltaBedCalibrationTools::calculate ()
{
    //Horizontal Radius Calibration*************************************************
    A  = PrinterConfig.alphaA;// parseFloat(document.getElementById('A').value);
    B  = PrinterConfig.alphaB;// parseFloat(document.getElementById('B').value);
    C  = PrinterConfig.alphaC;// parseFloat(document.getElementById('C').value);
    DA = PrinterConfig.deltaRadiusA;// parseFloat(document.getElementById('DA').value);
    DB = PrinterConfig.deltaRadiusB;// parseFloat(document.getElementById('DB').value);
    DC = PrinterConfig.deltaRadiusC;// parseFloat(document.getElementById('DC').value);
    HRad = PrinterConfig.horizontalRodRadius;// parseFloat(document.getElementById('HRad').value);
//	double measInver = parseFloat(document.getElementById('measInver').value);
    probeHeight = 0;//PrinterConfig.probeHeight;// parseFloat(document.getElementById('probeHeight').value);

    X = Height[0].X * measInver - probeHeight;// parseFloat(document.getElementById('X').value) * measInver - probeHeight;
    Y = Height[0].Y * measInver - probeHeight;// parseFloat(document.getElementById('Y').value) * measInver - probeHeight;
    Z = Height[0].Z * measInver - probeHeight;// parseFloat(document.getElementById('Z').value) * measInver - probeHeight;
    XOpp = Height[0].XOpp * measInver - probeHeight;// parseFloat(document.getElementById('XOpp').value) * measInver - probeHeight;
    YOpp = Height[0].YOpp * measInver - probeHeight;// parseFloat(document.getElementById('YOpp').value) * measInver - probeHeight;
    ZOpp = Height[0].ZOpp * measInver - probeHeight;// parseFloat(document.getElementById('ZOpp').value) * measInver - probeHeight;

    //add one to hrad
    double XTemp1 = Height[1].X * measInver - probeHeight;// parseFloat(document.getElementById('X1').value) * measInver - probeHeight;
    double YTemp1 = Height[1].Y * measInver - probeHeight;// parseFloat(document.getElementById('Y1').value) * measInver - probeHeight;
    double ZTemp1 = Height[1].Z * measInver - probeHeight;// parseFloat(document.getElementById('Z1').value) * measInver - probeHeight;
    double XOppTemp1 = Height[1].XOpp * measInver - probeHeight;// parseFloat(document.getElementById('XOpp1').value) * measInver - probeHeight;
    double YOppTemp1 = Height[1].YOpp * measInver - probeHeight;// parseFloat(document.getElementById('YOpp1').value) * measInver - probeHeight;
    double ZOppTemp1 = Height[1].ZOpp * measInver - probeHeight;// parseFloat(document.getElementById('ZOpp1').value) * measInver - probeHeight;


    double HRadRatio;
//    double newA;
//    double newB;
//    double newC;
//    double newDA;
//    double newDB;
//    double newDC;
//    double newHRad;

    measInver = 1.0;
    accuracy = 0.001;

    //Find the percentages for XYZ Offset
    double xOppositePercentage = 0.5;//= parseFloat(document.getElementById('xOppositePercentage').value);
    double yzPercentage = 0.25;//parseFloat(document.getElementById('yzPercentage').value);
    double X2 = Height[2].X * measInver - probeHeight;// parseFloat(document.getElementById('X2').value) * measInver - probeHeight;
    double Y2 = Height[2].Y * measInver - probeHeight;// parseFloat(document.getElementById('Y2').value) * measInver - probeHeight;
    double Z2 = Height[2].Z * measInver - probeHeight;// parseFloat(document.getElementById('Z2').value) * measInver - probeHeight;
    double XOpp2 = Height[2].XOpp * measInver - probeHeight;// parseFloat(document.getElementById('XOpp2').value) * measInver - probeHeight;
    double YOpp2 = Height[2].YOpp * measInver - probeHeight;// parseFloat(document.getElementById('YOpp2').value) * measInver - probeHeight;
    double ZOpp2 = Height[2].ZOpp * measInver - probeHeight;// parseFloat(document.getElementById('ZOpp2').value) * measInver - probeHeight;

    double yOppositePercentage = 0.5;//parseFloat(document.getElementById('yOppositePercentage').value);
    double xzPercentage = 0.25;//parseFloat(document.getElementById('xzPercentage').value);
    double X3 = Height[3].X * measInver - probeHeight;// parseFloat(document.getElementById('X3').value) * measInver - probeHeight;
    double Y3 = Height[3].Y * measInver - probeHeight;// parseFloat(document.getElementById('Y3').value) * measInver - probeHeight;
    double Z3 = Height[3].Z * measInver - probeHeight;//parseFloat(document.getElementById('Z3').value) * measInver - probeHeight;
    double XOpp3 = Height[3].XOpp * measInver - probeHeight;//parseFloat(document.getElementById('XOpp3').value) * measInver - probeHeight;
    double YOpp3 = Height[3].YOpp * measInver - probeHeight;//parseFloat(document.getElementById('YOpp3').value) * measInver - probeHeight;
    double ZOpp3 = Height[3].ZOpp * measInver - probeHeight;//parseFloat(document.getElementById('ZOpp3').value) * measInver - probeHeight;

    double zOppositePercentage = 0.5;//parseFloat(document.getElementById('zOppositePercentage').value);
    double xyPercentage = 0.25;//parseFloat(document.getElementById('xyPercentage').value);
    double X4 = Height[4].X * measInver - probeHeight;//parseFloat(document.getElementById('X4').value) * measInver - probeHeight;
    double Y4 = Height[4].Y * measInver - probeHeight;//parseFloat(document.getElementById('Y4').value) * measInver - probeHeight;
    double Z4 = Height[4].Z * measInver - probeHeight;//parseFloat(document.getElementById('Z4').value) * measInver - probeHeight;
    double XOpp4 = Height[4].XOpp * measInver - probeHeight;//parseFloat(document.getElementById('XOpp4').value) * measInver - probeHeight;
    double YOpp4 = Height[4].YOpp * measInver - probeHeight;//parseFloat(document.getElementById('YOpp4').value) * measInver - probeHeight;
    double ZOpp4 = Height[4].ZOpp * measInver - probeHeight;//parseFloat(document.getElementById('ZOpp4').value) * measInver - probeHeight;

    double xOppPerc = xOppositePercentage;
    double yzPerc = yzPercentage;

    double yOppPerc = yOppositePercentage;
    double xzPerc = xzPercentage;

    double zOppPerc = zOppositePercentage;
    double xyPerc = xyPercentage;

    if (xOppositePercentage > 0) {
        xOppPerc = xOppositePercentage;
        yzPerc = yzPercentage;

        yOppPerc = yOppositePercentage;
        xzPerc = xzPercentage;

        zOppPerc = zOppositePercentage;
        xyPerc = xyPercentage;
    }
    else {
        xOppPerc = getPercentagesOpp('X', X, Y, Z, XOpp, YOpp, ZOpp, X2, Y2, Z2, XOpp2, YOpp2, ZOpp2);
        yzPerc   = getPercentagesAll('X', X, Y, Z, XOpp, YOpp, ZOpp, X2, Y2, Z2, XOpp2, YOpp2, ZOpp2);

        yOppPerc = getPercentagesOpp('Y', X, Y, Z, XOpp, YOpp, ZOpp, X3, Y3, Z3, XOpp3, YOpp3, ZOpp3);
        xzPerc   = getPercentagesAll('Y', X, Y, Z, XOpp, YOpp, ZOpp, X3, Y3, Z3, XOpp3, YOpp3, ZOpp3);

        zOppPerc = getPercentagesOpp('Z', X, Y, Z, XOpp, YOpp, ZOpp, X4, Y4, Z4, XOpp4, YOpp4, ZOpp4);
        xyPerc   = getPercentagesAll('Z', X, Y, Z, XOpp, YOpp, ZOpp, X4, Y4, Z4, XOpp4, YOpp4, ZOpp4);
//        document.getElementById('xOppositePercentage').value = xOppPerc;
//        document.getElementById('yzPercentage').value = yzPerc;
//        document.getElementById('yOppositePercentage').value = yOppPerc;
//        document.getElementById('xzPercentage').value = xzPerc;
//        document.getElementById('zOppositePercentage').value = yOppPerc;
//        document.getElementById('xyPercentage').value = xyPerc;
    }

    teLog->appendPlainText(QString("Tower Offset Percentages: %1, %2, %3, %4, %5, %6").arg(
                                   QString::number(xOppPerc,'f',3),
                                   QString::number(yzPerc,'f',3),
                                   QString::number(yOppPerc,'f',3),
                                   QString::number(xzPerc,'f',3),
                                   QString::number(zOppPerc,'f',3),
                                   QString::number(xyPerc,'f',3)));

    //find the percentages for diagonal rod offset
    double XTemp5 = Height[5].X * measInver - probeHeight;//parseFloat(document.getElementById('X5').value) * measInver;
    double YTemp5 = Height[5].Y * measInver - probeHeight;//parseFloat(document.getElementById('Y5').value) * measInver;
    double ZTemp5 = Height[5].Z * measInver - probeHeight;//parseFloat(document.getElementById('Z5').value) * measInver;
    double XOppTemp5 = Height[5].XOpp * measInver - probeHeight;//parseFloat(document.getElementById('XOpp5').value) * measInver;
    double YOppTemp5 = Height[5].YOpp * measInver - probeHeight;//parseFloat(document.getElementById('YOpp5').value) * measInver;
    double ZOppTemp5 = Height[5].ZOpp * measInver - probeHeight;//parseFloat(document.getElementById('ZOpp5').value) * measInver;

    double delTower = 0.13083;//parseFloat(document.getElementById('deltaTower').value);
    double delOpp = 0.21083;//parseFloat(document.getElementById('deltaOpp').value);
    double diagonalRod = PrinterConfig.diagonalRodLength;// parseFloat(document.getElementById('diagonalRod').value);

    double deltaTower = delTower;
    double deltaOpp = delOpp;
    if (delTower > 0 && delOpp > 0) {
        deltaTower = delTower;
        deltaOpp = delOpp;
    }
    else {
        deltaTower = ((X - XTemp5) + (Y - YTemp5) + (Z - ZTemp5)) / 3;
        deltaOpp = ((XOpp - XOppTemp5) + (YOpp - YOppTemp5) + (ZOpp - ZOppTemp5)) / 3;

//        document.getElementById('deltaTower').value = deltaTower; // 1/8
//        document.getElementById('deltaOpp').value = deltaOpp; // 1/4
    }

    deltaTower = abs(deltaTower);
    deltaOpp = abs(deltaOpp);


//HRad is calibrated by increasing the outside edge of the glass by the average differences, this should balance the values with a central point of around zero
    //calculate HRad Ratio
    double HRadManual = -0.5;// parseFloat(document.getElementById('HRadManual').value);
    if (HRadManual > 0 || HRadManual < 0) {
        HRadRatio = HRadManual;
 //       teLog->appendPlainText(QString("Hrad Ratio: %1").arg(QString::number(HRadRatio,'f',3)));
    } else {
        HRadRatio = -(abs((XTemp1 - X) + (YTemp1 - Y) + (ZTemp1 - Z) + (XOppTemp1 - XOpp) + (YOppTemp1 - YOpp) + (ZOppTemp1 - ZOpp))) / 6;
//        HRadRatio = HRadRatio.toFixed(4);
//        console.log("Hrad Ratio: ", HRadRatio);
//        document.getElementById('HRadManual').value = HRadRatio;
    }
    teLog->appendPlainText(QString("Hrad Ratio: %1").arg(QString::number(HRadRatio,'f',4)));

    HRadSA = ((X + XOpp + Y + YOpp + Z + ZOpp) / 6);
//    HRadSA = parseFloat(HRadSA.toFixed(4));
    HRad = HRad + (HRadSA / HRadRatio);
    teLog->appendPlainText(QString("HRad change in mm: %1").arg(QString::number(HRadSA,'f',4)));
//    console.log("HRad change in mm: ", HRadSA);
    X = X - HRadSA;
    Y = Y - HRadSA;
    Z = Z - HRadSA;
    XOpp = XOpp - HRadSA;
    YOpp = YOpp - HRadSA;
    ZOpp = ZOpp - HRadSA;


    X = checkZero(X);
    Y = checkZero(Y);
    Z = checkZero(Z);
    XOpp = checkZero(XOpp);
    YOpp = checkZero(YOpp);
    ZOpp = checkZero(ZOpp);
    //modify XYZ,O values to match the new horizontal radius
    teLog->appendPlainText(QString("Tower heights after horizontal radius calibration: %1,%2,%3,%4,%5,%6").arg(
                               QString::number(X,'f',4),
                               QString::number(XOpp,'f',4),
                               QString::number(Y,'f',4),
                               QString::number(YOpp,'f',4),
                               QString::number(Z,'f',4),
                               QString::number(ZOpp,'f',4)));


    //checks the calibration of the horizontal radius
    calibVerify();

    double DASA = ((X + XOpp) / 2);
    double DBSA = ((Y + YOpp) / 2);
    double DCSA = ((Z + ZOpp) / 2);
    DA = DA + ((DASA) / HRadRatio);
    DB = DB + ((DBSA) / HRadRatio);
    DC = DC + ((DCSA) / HRadRatio);

    if (DA > 1 || DA < -1) {
        DASP->setValue(DA);//DA.toPrecision(4);
        //document.getElementById('DA').value = DA.toPrecision(4);
    } else if (DA > 0 && DA < 0.001) {
        DASP->setValue(0.0);
//        document.getElementById('DA').value = 0;
        DA = 0;
    } else if (DA < 0 && DA > -0.001) {
        DASP->setValue(0.0);
//        document.getElementById('DA').value = 0;
        DA = 0;
    } else {
        DASP->setValue(DA);//DA.toPrecision(3);
//        document.getElementById('DA').value = DA.toPrecision(3);
    }
    if (DB > 1 || DB < -1) {
        DBSP->setValue(DB);//DA.toPrecision(4);
//        document.getElementById('DB').value = DB.toPrecision(4);
    } else if (DB > 0 && DB < 0.001) {
        DBSP->setValue(0.0);
//        document.getElementById('DB').value = 0;
        DB = 0;
    } else if (DB < 0 && DB > -0.001) {
        DBSP->setValue(0.0);
//        document.getElementById('DB').value = 0;
        DB = 0;
    } else {
        DBSP->setValue(DB);//DB.toPrecision(3);
//        document.getElementById('DB').value = DB.toPrecision(3);
    }
    if (DC > 1 || DC < -1) {
        DCSP->setValue(DC);//DC.toPrecision(4);
//        document.getElementById('DC').value = DC.toPrecision(4);
    } else if (DC > 0 && DC < 0.001) {
        DCSP->setValue(0.0);
//        document.getElementById('DC').value = 0;
        DC = 0;
    } else if (DC < 0 && DC > -0.001) {
        DCSP->setValue(0.0);
//        document.getElementById('DC').value = 0;
        DC = 0;
    } else {
        DCSP->setValue(DC); // DC.toPrecision(3);
//        document.getElementById('DC').value = DC.toPrecision(3);
    }
    ////////////////////////////////////////////////////////////////////////////////
    //Tower Offset Calibration******************************************************
    stepsPerMM = PrinterConfig.stepPerMM;
//    var getSteps = document.getElementById('stepsPerMM').value;
//    if (getSteps > 0) {
//        var stepsPerMM = getSteps;
//    } else {
//        var motorStepAngle = document.getElementById('stepSize').value;
//        var driverMicroStepping = document.getElementById('microStepping').value;
//        var beltPitch = document.getElementById('beltPitch').value;
//        var toothCount = document.getElementById('toothCount').value;
//        stepsPerMM = ((360 / motorStepAngle) * driverMicroStepping)/(beltPitch * toothCount);
//        document.getElementById('stepsPerMM').value = stepsPerMM;
//    }

    //balance axes
    offsetX = PrinterConfig.xEndstopOffset ;//parseFloat(document.getElementById('offsetX').value);
    offsetY = PrinterConfig.yEndstopOffset ;//parseFloat(document.getElementById('offsetY').value);
    offsetZ = PrinterConfig.zEndstopOffset ;//parseFloat(document.getElementById('offsetZ').value);
    double offsetXYZ = 1 / 0.66;

    //XYZ offset
    //X
    double xxOppPerc = 0.5;
    xyPerc = 0.25;
    double xyOppPerc = 0.25;
    xzPerc = 0.25;
    double xzOppPerc = 0.25;

    //Y
    double yyOppPerc = 0.5;
    double yxPerc    = 0.25;
    double yxOppPerc = 0.25;
    yzPerc = 0.25;
    double yzOppPerc = 0.25;

    //Z
    double zzOppPerc = 0.5;
    double zxPerc    = 0.25;
    double zxOppPerc = 0.25;
    double zyPerc    = 0.25;
    double zyOppPerc = 0.25;

    Q_UNUSED(yzOppPerc);
    Q_UNUSED(zxOppPerc);


    double j = 0;
    //correction of one tower allows the diagonal rod to respond more accurately
    while (j < 1)
    {
        qApp->processEvents();
        double theoryX = offsetX + X * stepsPerMM * offsetXYZ;

        //correction of one tower allows for XY dimensional accuracy
        if (X > 0) {
            //if x is positive
            offsetX = offsetX + X * stepsPerMM * offsetXYZ;

            XOpp = XOpp + (X * xxOppPerc);//0.5
            Z = Z + (X * xzPerc);//0.25
            Y = Y + (X * xyPerc);//0.25
            ZOpp = ZOpp - (X * xzOppPerc);//0.25
            YOpp = YOpp - (X * xyOppPerc);//0.25
            X = X - X;
        }
        else if (theoryX > 0 && X < 0){
            //if x is negative and can be decreased
            offsetX = offsetX + X * stepsPerMM * offsetXYZ;

            XOpp = XOpp + (X * xxOppPerc);//0.5
            Z = Z + (X * xzPerc);//0.25
            Y = Y + (X * xyPerc);//0.25
            ZOpp = ZOpp - (X * xzOppPerc);//0.25
            YOpp = YOpp - (X * xyOppPerc);//0.25
            X = X - X;
        }
        else{
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

        double theoryY = offsetY + Y * stepsPerMM * offsetXYZ;

        //Y
        if (Y > 0){
            offsetY = offsetY + Y * stepsPerMM * offsetXYZ;

            YOpp = YOpp + (Y * yyOppPerc);
            X = X + (Y * yxPerc);
            Z = Z + (Y * yxPerc);
            XOpp = XOpp - (Y * yxOppPerc);
            ZOpp = ZOpp - (Y * yxOppPerc);
            Y = Y - Y;
        }
        else if (theoryY > 0 && Y < 0){
            offsetY = offsetY + Y * stepsPerMM * offsetXYZ;

            YOpp = YOpp + (Y * yyOppPerc);
            X = X + (Y * yxPerc);
            Z = Z + (Y * yxPerc);
            XOpp = XOpp - (Y * yxOppPerc);
            ZOpp = ZOpp - (Y * yxOppPerc);
            Y = Y - Y;
        }
        else{
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

        double theoryZ = offsetZ + Z * stepsPerMM * offsetXYZ;

        //Z
        if (Z > 0){
            offsetZ = offsetZ + Z * stepsPerMM * offsetXYZ;

            ZOpp = ZOpp + (Z * zzOppPerc);
            X = X + (Z * zxPerc);
            Y = Y + (Z * zyPerc);
            XOpp = XOpp - (Z * yxOppPerc);
            YOpp = YOpp - (Z * zyOppPerc);
            Z = Z - Z;
        }
        else if (theoryZ > 0 && Z < 0){
            offsetZ = offsetZ + Z * stepsPerMM * offsetXYZ;

            ZOpp = ZOpp + (Z * zzOppPerc);
            X = X + (Z * zxPerc);
            Y = Y + (Z * zyPerc);
            XOpp = XOpp - (Z * yxOppPerc);
            YOpp = YOpp - (Z * zyOppPerc);
            Z = Z - Z;
        }
        else{
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

        teLog->appendPlainText(
                    QString("Virtual heights: X:%1, XOpp:%2 Y:%3, YOpp:%4, Z:%5, and ZOpp:%6").arg(
                        QString::number(X,'f',3),
                        QString::number(XOpp,'f',3),
                        QString::number(Y,'f',3),
                        QString::number(YOpp,'f',3),
                        QString::number(Z,'f',3),
                        QString::number(ZOpp,'f',3)));
        accuracy = 0.005;
        if (X < accuracy && X > -accuracy && Y < accuracy && Y > -accuracy && Z < accuracy && Z > -accuracy){
            j = 1;
        }
    }

//    document.getElementById('offsetX').value = Math.round(offsetX);
//    document.getElementById('offsetY').value = Math.round(offsetY);
//    document.getElementById('offsetZ').value = Math.round(offsetZ);


    //Alpha Rotation Calibration****************************************************
    //Iterate through until balanced
    //change loop to be based on difference between XOpp, YOpp, ZOpp
    for (int i = 0; i < 1;) {
        //X Alpha Rotation
        qApp->processEvents();
        if (YOpp > ZOpp) {
            double ZYOppAvg = (YOpp - ZOpp) / 2;
            A = A + (ZYOppAvg * 1.725); // (0.5/((diff y0 and z0 at X + 0.5)-(diff y0 and z0 at X = 0))) * 2 = 1.75
            YOpp = YOpp - ZYOppAvg;
            ZOpp = ZOpp + ZYOppAvg;
        }
        else if (YOpp < ZOpp) {
            double ZYOppAvg = (ZOpp - YOpp) / 2;

            A = A - (ZYOppAvg * 1.725);
            YOpp = YOpp + ZYOppAvg;
            ZOpp = ZOpp - ZYOppAvg;
        }

        //Y Alpha Rotation
        if (ZOpp > XOpp) {
            double XZOppAvg = (ZOpp - XOpp) / 2;
            B = B + (XZOppAvg * 1.725);
            ZOpp = ZOpp - XZOppAvg;
            XOpp = XOpp + XZOppAvg;
        }
        else if (ZOpp < XOpp) {
            double XZOppAvg = (XOpp - ZOpp) / 2;

            B = B - (XZOppAvg * 1.725);
            ZOpp = ZOpp + XZOppAvg;
            XOpp = XOpp - XZOppAvg;
        }
        //Z Alpha Rotation
        if (XOpp > YOpp) {
            double YXOppAvg = (XOpp - YOpp) / 2;
            C = C + (YXOppAvg * 1.725);
            XOpp = XOpp - YXOppAvg;
            YOpp = YOpp + YXOppAvg;
        } else if (XOpp < YOpp) {
            double YXOppAvg = (YOpp - XOpp) / 2;

            C = C - (YXOppAvg * 1.725);
            XOpp = XOpp + YXOppAvg;
            YOpp = YOpp - YXOppAvg;
        }
        //determine if value is close enough
        double hTow = qMax(XOpp, qMax(YOpp, ZOpp));
        double lTow = qMin(XOpp, qMin(YOpp, ZOpp));
        double towDiff = hTow - lTow;
//        towDiff = parseFloat(towDiff.toFixed(6));
        if (towDiff < 0.0001) {
            i = 1;
            teLog->appendPlainText(QString("Alpha Rotation Calibrated to: %1").arg(QString::number(towDiff,'f',3)));
        }
    }
    //Diagonal Rod Calibration******************************************************
    XTemp5 = Height[5].X;// parseFloat(document.getElementById('X5').value);
    YTemp5 = Height[5].Y;// parseFloat(document.getElementById('Y5').value);
    ZTemp5 = Height[5].Z;// parseFloat(document.getElementById('Z5').value);
    XOppTemp5 = Height[5].XOpp;// parseFloat(document.getElementById('XOpp5').value);
    YOppTemp5 = Height[5].YOpp;// parseFloat(document.getElementById('YOpp5').value);
    ZOppTemp5 = Height[5].ZOpp;// parseFloat(document.getElementById('ZOpp5').value);
    //var deltaTower = 0.13083;
    //var deltaOpp = 0.21083;
//    var delTower = parseFloat(document.getElementById('deltaTower').value);
//    var delOpp = parseFloat(document.getElementById('deltaOpp').value);
//    var diagonalRod = parseFloat(document.getElementById('diagonalRod').value);
    if (delTower > 0 && delOpp > 0) {
        deltaTower = delTower;
        deltaOpp = delOpp;
    }
    else {
        deltaTower = ((X - XTemp5) + (Y - YTemp5) + (Z - ZTemp5)) / 3;
        deltaOpp = ((XOpp - XOppTemp5) + (YOpp - YOppTemp5) + (ZOpp - ZOppTemp5)) / 3;
//        document.getElementById('deltaTower').value = deltaTower; // 1/8
//        document.getElementById('deltaOpp').value = deltaOpp; // 1/4
    }
    deltaTower = abs(deltaTower);
    deltaOpp = abs(deltaOpp);
    double diagChange = 1 / deltaOpp;
    double towOppDiff = deltaTower / deltaOpp; //0.5
    //increase decreases all towers
    //decrease increases all towers
    for (int i = 0; i < 1;) {
        qApp->processEvents();
        double XYZOpp = (XOpp + YOpp + ZOpp) / 3;
        diagonalRod = diagonalRod + (XYZOpp * diagChange);
        X = X - towOppDiff * XYZOpp;
        Y = Y - towOppDiff * XYZOpp;
        Z = Z - towOppDiff * XYZOpp;
        XOpp = XOpp - XYZOpp;
        YOpp = YOpp - XYZOpp;
        ZOpp = ZOpp - XYZOpp;
        XYZOpp = (XOpp + YOpp + ZOpp) / 3;
        XYZOpp = checkZero(XYZOpp);

        double XYZ = (X + Y + Z) / 3;
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
//            diagonalRod = parseFloat(diagonalRod.toFixed(6));
            teLog->appendPlainText(QString("Diagonal Rod Calibrated to: %1").arg(QString::number(diagonalRod,'f',6)));
//            document.getElementById('diagonalRod').value = diagonalRod.toFixed(3);
        }
    }
    //send obtained values back to the document*************************************
//    document.getElementById('A').value = A.toFixed(3);
//    document.getElementById('B').value = B.toFixed(3);
//    document.getElementById('C').value = C.toFixed(3);

//    document.getElementById('HRad').value = HRad.toFixed(3);
//    document.getElementById('HRadManual').value = HRadRatio;
    teLog->appendPlainText(QString("New tower heights: %1, %2, %3, %4, %5, %6").arg(
                                   QString::number(checkZero(X),'f',3),
                                   QString::number(checkZero(XOpp),'f',3),
                                   QString::number(checkZero(Y),'f',3),
                                   QString::number(checkZero(YOpp),'f',3),
                                   QString::number(checkZero(Z),'f',3),
                                   QString::number(checkZero(ZOpp),'f',3)));
    teLog->appendPlainText(QString("New horizontal radius: %1").arg(QString::number(HRad,'f',4)));
    teLog->appendPlainText(QString("New delta radii: %1, %2, %3").arg(
                                   QString::number(DA,'f',3),
                                   QString::number(DB,'f',3),
                                   QString::number(DC,'f',3)));
    teLog->appendPlainText(QString("New alpha rotation: %1, %2, %3").arg(
                                   QString::number(A,'f',3),
                                   QString::number(B,'f',3),
                                   QString::number(C,'f',3)));
}


double CDeltaBedCalibrationTools::getPercentagesOpp(char _XYZ, double _X    , double _Y    , double _Z,
                                                               double _XOpp , double _YOpp , double _ZOpp,
                                                               double _X2   , double _Y2   , double _Z2,
                                                               double _XOpp2, double _YOpp2, double _ZOpp2)
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

double CDeltaBedCalibrationTools::getPercentagesAll(char _XYZ, double _X    , double _Y    , double _Z,
                                                               double _XOpp , double _YOpp , double _ZOpp,
                                                               double _X2   , double _Y2   , double _Z2,
                                                               double _XOpp2, double _YOpp2, double _ZOpp2)
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

void CDeltaBedCalibrationTools::calibVerify()
{
    //checks if the value obtained by the new HRad is correct
    if (HRadSA < 0.1) {
        if (HRadSA < 0.0001) {
            HRadSA = 0;
        }
        teLog->appendPlainText(QString("Horizontal Radius Calibration Success - %1").arg(QString::number(HRadSA,'f',4)));
    }
    else {
        teLog->appendPlainText(QString("Horizontal Radius Calibration Failure - %1").arg(QString::number(HRadSA,'f',4)));
    }
}

double CDeltaBedCalibrationTools::checkZero(double value)
{
    if (value > -0.0001 && value < 0.0001) return 0;
    else return value;
}


QStringList CDeltaBedCalibrationTools::PrepareScript()
{
    QStringList list;
    int index = Printer.EEPROM->indexByStartName("Diagonal rod length");
    if (index>=0){
        list.append(Printer.EEPROM->at(index)->ToCmdString());
    }
    index = Printer.EEPROM->indexByStartName("Horizontal rod radius at");
    if (index>=0){
        list.append(Printer.EEPROM->at(index)->ToCmdString());
    }
    index = Printer.EEPROM->indexByStartName("Tower X endstop offset");
    if (index>=0){
        list.append(Printer.EEPROM->at(index)->ToCmdString());
    }
    index = Printer.EEPROM->indexByStartName("Tower Y endstop offset");
    if (index>=0){
        list.append(Printer.EEPROM->at(index)->ToCmdString());
    }
    index = Printer.EEPROM->indexByStartName("Tower Z endstop offset");
    if (index>=0){
        list.append(Printer.EEPROM->at(index)->ToCmdString());
    }
    return list;
}

//double SIN_1=sin(M_PI/6.0+M_PI/2.0);
//double COS_1=cos(M_PI/6.0+M_PI/2.0);

//Vertices.append(TVertex(dx*COS_1 - dy*SIN_1,dx*SIN_1 + dy*COS_1));

bool CDeltaBedCalibrationTools::gotoxyz ()
{
    double x,y,z;
    z=30;
    if (currentVertexIndex==0){ //X
        x = -TestPlaneRadius*cos(M_PI/6.0);
        y = -TestPlaneRadius*sin(M_PI/6.0);
    }
    else if (currentVertexIndex==1){ //Xopp
        x = TestPlaneRadius*cos(M_PI/6.0);
        y = TestPlaneRadius*sin(M_PI/6.0);
    }
    else if (currentVertexIndex==2){ //Y
        x = TestPlaneRadius*cos(M_PI/6.0);
        y = -TestPlaneRadius*sin(M_PI/6.0);
    }
    else if (currentVertexIndex==3){ //Yopp
        x = -TestPlaneRadius*cos(M_PI/6.0);
        y = TestPlaneRadius*sin(M_PI/6.0);
    }
    else if (currentVertexIndex==4){ //Z
        x = 0;
        y = TestPlaneRadius;
    }
    else if (currentVertexIndex==5){ //Zopp
        x = 0;
        y = -TestPlaneRadius;
    }
    else return false;
    step = GoToXYZ;
    Printer.sendGoToXYZ(x,y,z);
    return true;
}

void CDeltaBedCalibrationTools::readEeprom()
{
    CEePromRecord* record;
    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Steps per mm"));
        if (record) PrinterConfig.stepPerMM = record->FValue;

    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Z-probe height"));
        if (record) PrinterConfig.probeHeight = record->FValue;

    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Tower X endstop offset"));
        if (record) PrinterConfig.xEndstopOffset = record->IValue;
    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Tower Y endstop offset"));
        if (record) PrinterConfig.yEndstopOffset = record->IValue;
    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Tower Z endstop offset"));
        if (record) PrinterConfig.zEndstopOffset = record->IValue;

    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Alpha A(210)"));
        if (record) PrinterConfig.alphaA = record->FValue;
    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Alpha B(330)"));
        if (record) PrinterConfig.alphaB = record->FValue;
    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Alpha C(90)"));
        if (record) PrinterConfig.alphaC = record->FValue;

    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Delta Radius A(0)"));
        if (record) PrinterConfig.deltaRadiusA = record->FValue;
    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Delta Radius B(0)"));
        if (record) PrinterConfig.deltaRadiusB = record->FValue;
    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Delta Radius C(0)"));
        if (record) PrinterConfig.deltaRadiusC = record->FValue;

    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Diagonal rod length"));
        if (record) PrinterConfig.diagonalRodLength = record->FValue;

    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Horizontal rod radius at 0,0"));
        if (record) PrinterConfig.horizontalRodRadius = record->FValue;
}

void CDeltaBedCalibrationTools::slotCommandExecuted()
{
    mainLoop();
}

void CDeltaBedCalibrationTools::on_pbStart_clicked()
{
    if (state==Stoped){
        start();
        state= Active;
        step = ReadEEPROM;
        currentVertexIndex = 0; // Начало
        currentHeightIndex = 0;
        Printer.EEPROM->read();
    }
}

void CDeltaBedCalibrationTools::on_pbStop_clicked()
{
    if (state==Paused){
        state = Stoped;
        stop();
    }
    else{
        state = WaitStop;
    }
    updateControls();
}

void CDeltaBedCalibrationTools::slotReady(const QString&)
{
    if (state==Active)
        mainLoop();
}

void CDeltaBedCalibrationTools::updateControls()
{

}

CDeltaBedCalibrationTools::CDeltaBedCalibrationTools(QWidget *parent) :
    CBaseWindow(parent),Ui::dbct_wnd()
{
    setupUi(this);
    setWindowTitle("Delta Bed Calibration Tools");
    setWindowIcon(QIcon(":/images/opendact.png"));
    state = Stoped;
    step  = NoStep;
    currentVertexIndex = 0;
    currentHeightIndex = 0;
    TestPlaneRadius    = TEST_PLANE_RADIUS;
    TestProbeStartOffset = TEST_PROBE_START_OFFSET;
    connect (&Printer,SIGNAL(signalCommandExecuted()),this,SLOT(slotCommandExecuted()));
    connect (&Printer      ,SIGNAL(signalReady(QString)),this,SLOT(slotReady(QString)));
//--    connect (Printer.EEPROM,SIGNAL(signalReady(QString)),this,SLOT(slotReady(QString)));
}

CDeltaBedCalibrationTools::~CDeltaBedCalibrationTools()
{
}

void CDeltaBedCalibrationTools::on_pbCalibrate_clicked()
{
//    CDbctCalibrate calibrate;
//    calibrate.Calibrate(this);
}
