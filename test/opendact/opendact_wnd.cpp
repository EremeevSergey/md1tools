#include <QDebug>
#include "opendact_wnd.h"
#include "common.h"

void COpendactWnd::on_calibrateButton_clicked()
{
    if (state==Stoped){
        slotLogConsole("Calibration started",Qt::blue);
        start();
        setUserVariables();
        state= Active;
        step = ReadEEPROM;
        Printer.EEPROM->read();
        OpenDACT.iterationNum = 0;
        checkHeightsOnly = false;
//-------------------------------------------------
        EEPROM.tempEEPROMSet          = false;
        OpenDACT.checkHeights         = true;
        OpenDACT.EEPROMReadOnly       = false;
        OpenDACT.calibrationComplete  = false;
        OpenDACT.calibrationState     = true;
        OpenDACT.calibrationSelection = 0;
        OpenDACT.checkHeightsOnly     = false;
    //    OpenDACT.isCalibrating        = true;

        OpenDACT.calibrateInProgress = false;
        OpenDACT.wasZProbeHeightSet  = false;

        iteration           = 0;
        wasSet              = false;
        currentPosition     = 0;
        Heights.position    = 0;

        Printer.EEPROM->read();//EEPROMFunctions.readEEPROM();
    }
    else if (state==Active){
        state= WaitPaused;
    }
    else if (state==Paused){
        state= Active;
        mainLoop();
    }
    updateControls();
}

void COpendactWnd::mainLoop()
{
//    if (state==Active) qDebug() << "mainLoop(Active){";
//    else               qDebug() << "mainLoop(){";
    switch (state) {
    case Active:
        activeLoop();
        break;
    case WaitStop:
        state = Stoped;
        stop();
        break;
    case WaitPaused:
        state = Paused;
        break;
    default:
        break;
    }
    updateControls();
}
/*
 1. Считываем ЕЕПРОМ (ReadEEPROM)
 2. Измеряем Z-Probe (CheckZProbe)
 3. Измеряем высоты башень и противоположных им точек (CheckHieghts)
*/
void COpendactWnd::activeLoop()
{
//    qDebug() << "      activeLoop(){";
    switch (step){
    case ReadEEPROM:
        // Считали ЕЕПРОМ. Сохраняем начальные условия
        EEPROM.readEEPROM();
        setEEPROMGUIList();
//        EEPROM  .tempEEPROMSet = true;
//        OpenDACT.checkHeights  = true;
//        EEPROMReadCount++;
        if (UserVariables.probeChoice == "Z-Probe") step = CheckZProbe;
        else                                        step = CheckHieghts;
        iteration       = 0;
        currentPosition = 0;
        Heights.init();
        emit signalNextStep();
        break;
    case CheckZProbe:
        checkZProbe();
        break;
    case CheckHieghts:
        checkHeights();
        break;
    case Calibrate:
        calibrate ();
        break;
    case GoHome:
        state = WaitStop;
        emit signalNextStep();
        break;
    default:
        break;
    }
}

void COpendactWnd::checkZProbe   ()
{
//    qDebug() << "      checkZProbe(){";
    switch (iteration){
    case 0:
        EEPROM.zProbeHeight = 0;
        setEEPROMGUIList();
        if (EEPROM.sendEEPROM()<=0) // Перезаписываем в еепром
            emit signalNextStep();
        iteration++;
        break;
    case 1:
//        EEPROM.zProbeHeight = 0;
        Printer.cmdGoHomeAll();
        iteration++;
        break;
    case 2:
        Printer.cmdGoToXYZ(0,0,int(EEPROM.zMaxLength/6.0+0.5));
        iteration++;
        break;
    case 3:
        Printer.cmdGetZProbeValue();// probe();
        iteration++;
        break;
    case 4:
        EEPROM.zProbeHeight = int(EEPROM.zMaxLength/6+0.5) - Printer.ZProbe.Z;
        setEEPROMGUIList();
        if (EEPROM.sendEEPROM()<=0) // Перезаписываем в еепром
            emit signalNextStep();
        currentPosition = 0;
        iteration = 0;
        Heights.init();
        step = CheckHieghts;
        break;
    }
}

void COpendactWnd::checkHeights  ()
{
//    qDebug() << "      positionFlow(){";
    float probingHeight = UserVariables.probingHeight;
    float plateDiameter = UserVariables.plateDiameter;
    int pauseTimeSet = UserVariables.pauseTimeSet;
    float valueZ = 0.482F * plateDiameter;
    float valueXYLarge = 0.417F * plateDiameter;
    float valueXYSmall = 0.241F * plateDiameter;

    Q_UNUSED(pauseTimeSet);

//    Printer.sendCmd(QString("G0 F%1").arg(UserVariables.xySpeed * 60),false);//converts mm/s to mm/min
    switch (currentPosition){
    case 0:
        switch (iteration){
        case 0:
            //qDebug() << "        handleInput() 0-0";
            Printer.cmdGoHomeAll();
            iteration++;
            break;
        case 1:
            //qDebug() << "        handleInput() 0-1";
            Printer.cmdGoToXYZ(0,0,probingHeight);// "G1 Z" + probingHeight + " X0 Y0";
            iteration++;
            break;
        case 2:
            //qDebug() << "        handleInput() 0-2";
            Printer.cmdGetZProbeValue();// probe();
            iteration++;
            break;
        case 3:
            //qDebug() << "        handleInput() 0-3";
            Heights.setHeights(Printer.ZProbe.Z);
            setHeightsInvoke();
            //Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
            //                                 " X-" + valueXYLarge.ToString() +
            //                                 " Y-" + valueXYSmall.ToString());
            iteration++;
            Printer.cmdGoToXYZ(-valueXYLarge,-valueXYSmall,probingHeight);
            currentPosition++;
            iteration = 0;
        }
        break;
    case 1:
        switch (iteration){
        case 0:
            //qDebug() << "        handleInput() 1-0";
            Printer.cmdGetZProbeValue();// probe();
            iteration++;
            break;
        case 1:
            //qDebug() << "        handleInput() 1-1";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X-" + valueXYLarge.ToString() +
//                                                 " Y-" + valueXYSmall.ToString());
            Heights.setHeights(Printer.ZProbe.Z);
            setHeightsInvoke();
            Printer.cmdGoToXYZ(-valueXYLarge,-valueXYSmall,probingHeight);
            iteration++;
            break;
        case 2:
            //qDebug() << "        handleInput() 1-2";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() + " X0 Y0");
            Printer.cmdGoToXYZ(0,0,probingHeight);
            iteration++;
            break;
        case 3:
            //qDebug() << "        handleInput() 1-3";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X" + valueXYLarge.ToString() +
//                                                 " Y" + valueXYSmall.ToString());
            Printer.cmdGoToXYZ(valueXYLarge,valueXYSmall,probingHeight);
            currentPosition++;
            iteration = 0;
            break;
        }
        break;
    case 2:
        switch (iteration){
        case 0:
            //qDebug() << "        handleInput() 2-0";
            Printer.cmdGetZProbeValue();// probe();
            iteration++;
            break;
        case 1:
            //qDebug() << "        handleInput() 2-1";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X" + valueXYLarge.ToString() +
//                                                 " Y" + valueXYSmall.ToString());
            Heights.setHeights(Printer.ZProbe.Z);
            setHeightsInvoke();
            Printer.cmdGoToXYZ(valueXYLarge,valueXYSmall,probingHeight);
            iteration++;
            break;
        case 2:
            //qDebug() << "        handleInput() 2-2";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() + " X0 Y0");
            Printer.cmdGoToXYZ(0,0,probingHeight);
            iteration++;
            break;
        case 3:
            //qDebug() << "        handleInput() 2-3";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X" + valueXYLarge.ToString() +
//                                                 " Y-" + valueXYSmall.ToString());
            Printer.cmdGoToXYZ(valueXYLarge,-valueXYSmall,probingHeight);
            currentPosition++;
            iteration = 0;
            break;
        }
        break;
    case 3:
        switch (iteration){
        case 0:
            //qDebug() << "        handleInput() 3-0";
            Printer.cmdGetZProbeValue();// probe();
            iteration++;
            break;
        case 1:
            //qDebug() << "        handleInput() 3-1";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X" + valueXYLarge.ToString() +
//                                                 " Y-" + valueXYSmall.ToString());
            Heights.setHeights(Printer.ZProbe.Z);
            setHeightsInvoke();
            Printer.cmdGoToXYZ(valueXYLarge,-valueXYSmall,probingHeight);
            iteration++;
            break;
        case 2:
            //qDebug() << "        handleInput() 3-2";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() + " X0 Y0");
            Printer.cmdGoToXYZ(0,0,probingHeight);
            iteration++;
            break;
        case 3:
            //qDebug() << "        handleInput() 3-3";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X-" + valueXYLarge.ToString() +
//                                                 " Y" + valueXYSmall.ToString());
            Printer.cmdGoToXYZ(-valueXYLarge,valueXYSmall,probingHeight);
            currentPosition++;
            iteration = 0;
            break;
        }
        break;
    case 4:
        switch (iteration){
        case 0:
            //qDebug() << "        handleInput() 4-0";
            Printer.cmdGetZProbeValue();// probe();
            iteration++;
            break;
        case 1:
            //qDebug() << "        handleInput() 4-1";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X-" + valueXYLarge.ToString() +
//                                                 " Y" + valueXYSmall.ToString());
            Heights.setHeights(Printer.ZProbe.Z);
            setHeightsInvoke();
            Printer.cmdGoToXYZ(-valueXYLarge,valueXYSmall,probingHeight);
            iteration++;
            break;
        case 2:
            //qDebug() << "        handleInput() 4-2";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() + " X0 Y0");
            Printer.cmdGoToXYZ(0,0,probingHeight);
            iteration++;
            break;
        case 3:
            //qDebug() << "        handleInput() 4-3";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X0 Y" + valueZ.ToString());
            Printer.cmdGoToXYZ(0,valueZ,probingHeight);
            currentPosition++;
            iteration = 0;
            break;
        }
        break;
    case 5:
        switch (iteration){
        case 0:
            //qDebug() << "        handleInput() 5-0";
            Printer.cmdGetZProbeValue();// probe();
            iteration++;
            break;
        case 1:
            //qDebug() << "        handleInput() 5-1";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X0 Y" + valueZ.ToString());
            Heights.setHeights(Printer.ZProbe.Z);
            setHeightsInvoke();
            Printer.cmdGoToXYZ(0,valueZ,probingHeight);
            iteration++;
            break;
        case 2:
            //qDebug() << "        handleInput() 5-2";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X0 Y-" + valueZ.ToString());
            Printer.cmdGoToXYZ(0,-valueZ,probingHeight);
            currentPosition++;
            iteration = 0;
            break;
        }
        break;
    case 6:
        switch (iteration){
        case 0:
            //qDebug() << "        handleInput() 6-0";
            Printer.cmdGetZProbeValue();// probe();
            iteration++;
            break;
        case 1:
            //qDebug() << "        handleInput() 6-1";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X0 Y-" + valueZ.ToString());
            Heights.setHeights(Printer.ZProbe.Z);
            setHeightsInvoke();
            Printer.cmdGoToXYZ(0,-valueZ,probingHeight);
            iteration++;
            break;
        case 2:
            //qDebug() << "        handleInput() 6-2";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() + " X0 Y0");
            Printer.cmdGoToXYZ(0,0,probingHeight);
            iteration++;
            if (OpenDACT.calibrateInProgress == false){ iteration++; }
            break;
        case 3:
            //qDebug() << "        handleInput() 6-3";
//                Connection._serialPort.WriteLine("G1 Z" + Convert.ToInt32(EEPROM.zMaxLength / 3) + " X0 Y0");
            Printer.cmdGoToXYZ(0,0,int(EEPROM.zMaxLength / 3+0.5));
            iteration++;
            Heights.printHeights();
            setHeightsInvoke();
            if (checkHeightsOnly){
                step = GoHome;
                Printer.cmdGoHomeAll();
            }
            else step = Calibrate;
            iteration=0;
            break;
        case 4:
            //qDebug() << "        handleInput() 6-4";
//            OpenDACT.checkHeights = false;
            Heights.printHeights();
            setHeightsInvoke();
            if (checkHeightsOnly){
                step = GoHome;
                Printer.cmdGoHomeAll();
            }
            else step = Calibrate;
            iteration=0;
            emit signalNextStep();
            break;
        }
        break;
    }//end switch
}

void COpendactWnd::calibrate     ()
{
//    qDebug() << "      calibrate(){";
    slotLogConsole(QString("Calibration Iteration Number: %1").arg(
                              QString::number(OpenDACT.iterationNum)
                              ),Qt::green);
    if (OpenDACT.calibrate()) {
        step = GoHome;
        slotLogConsole("Calibration Complete",Qt::blue);
        Printer.cmdGoHomeAll();
    }
    else{
        step = CheckZProbe;
        currentPosition = 0;
        iteration = 0;
        Heights.init();
        setEEPROMGUIList();
        if (EEPROM.sendEEPROM()<=0) // Перезаписываем в еепром
            emit signalNextStep();
    }
//    bool ret;
//    switch (iteration) {
//    case 0:
//        consoleMain->appendPlainText(QString("Calibration Iteration Number: %1").arg(
//                                  QString::number(OpenDACT.iterationNum)
//                                  ));
//        if (OpenDACT.calibrate()) {
//            step = GoHome;
//            consoleMain->appendPlainText("Calibration Complete");
//        }
//        else{
//            iteration++;
//            step = CheckZProbe;
//            currentPosition = 0;
//            iteration = 0;
//            Heights.init();
//        }
//        setEEPROMGUIList();
//        EEPROM.sendEEPROM();

//        break;
//    case 1:
//        if (OpenDACT.calibrationState == false){
//            Printer.cmdGoHomeAll();//GCode.homeAxes();
//            OpenDACT.calibrationComplete = true;
//            consoleMain->appendPlainText("Calibration Complete");
//            state = Stoped;

//            //end calibration
//        }
//        else{
//            state = Stoped;// пока
//            emit signalNextStep();
//        }
//        iteration=0;
//        break;
//    default:
//        break;
//    }
}

void COpendactWnd::activeLoopOld    ()
{
    if      (step==ReadEEPROM) qDebug() << "  activeLoop(ReadEEPROM){";
    else if (step==Process) qDebug() << "  activeLoop(Process){";
    else                       qDebug() << "  activeLoop(){";
    switch (step){
    case ReadEEPROM:
        // Считали ЕЕПРОМ. Сохраняем начальные условия
        EEPROM.readEEPROM();
        setEEPROMGUIList();
        EEPROM  .tempEEPROMSet = true;
        OpenDACT.checkHeights  = true;
//        EEPROMReadCount++;
        step = Process;         // пока
    case Process:
        handleInput("",true);
        break;
    case GoHome:
        // Пришли домой теперь надо промерить вершины
        break;
    case GoToXYZ:
        // Измеряем расстояние до стола
        break;
    case ExecuteCommand:
        break;
    case ZProbe:
        break;
    default:
        break;
    }
   qDebug() << "  } //activeLoop()";
}

void COpendactWnd::handleInput(const QString& message, bool canMove)
{
    Q_UNUSED(message);
    qDebug() << "    handleInput(){";
    if (OpenDACT.checkHeights == true && EEPROM.tempEEPROMSet == true &&
            OpenDACT.calibrateInProgress == false && OpenDACT.EEPROMReadOnly == false){
        if (UserVariables.probeChoice == "Z-Probe" && OpenDACT.wasZProbeHeightSet == false && wasSet == true){
//            if (HeightFunctions.parseZProbe(message) != 1000)
//            {
                EEPROM.zProbeHeight = int(EEPROM.zMaxLength / 6+0.5) - Printer.ZProbe.Z;//HeightFunctions.parseZProbe(message));
                OpenDACT.wasZProbeHeightSet = true;
                setEEPROMGUIList();
                EEPROM.sendEEPROM();
//            }
        }
        else if (canMove == true){
            positionFlowOld();
        }
        else if (/*HeightFunctions.parseZProbe(message) != 1000 &&*/ OpenDACT.heightsSet == false){
            Heights.setHeights(Printer.ZProbe.Z);
        }
    }
    else if (OpenDACT.calibrationState == true && OpenDACT.calibrateInProgress == false && OpenDACT.checkHeights == false &&
             EEPROM.tempEEPROMSet == true && OpenDACT.EEPROMReadOnly == false &&
             OpenDACT.heightsSet == true){
//        Program.mainFormTest.setHeightsInvoke();

        if (OpenDACT.calibrationState == true && OpenDACT.checkHeightsOnly == false){
            OpenDACT.calibrateInProgress = true;
            if (UserVariables.advancedCalibration == false || OpenDACT.isHeuristicComplete == true){
                consoleMain->appendPlainText(QString("Calibration Iteration Number: %1").arg(
                                          QString::number(OpenDACT.iterationNum)
                                          ));
                OpenDACT.calibrate();

                setEEPROMGUIList();
                EEPROM.sendEEPROM();

                if (OpenDACT.calibrationState == false){
                    Printer.cmdGoHomeAll();//GCode.homeAxes();
                    OpenDACT.calibrationComplete = true;
                    consoleMain->appendPlainText("Calibration Complete");
                    //end calibration
                }
            }
            else{
                consoleMain->appendPlainText(QString("Heuristic Step: %1").arg(
                                          QString::number(UserVariables.advancedCalCount,'f',3)
                                          ));
                OpenDACT.heuristicLearning();
                setEEPROMGUIList();
                EEPROM.sendEEPROM();
            }


            OpenDACT.calibrateInProgress = false;
        }
        else{
            if (UserVariables.probeChoice == "FSR"){
                EEPROM.zMaxLength -= UserVariables.FSROffset;
                consoleMain->appendPlainText("Setting Z Max Length with adjustment for FSR");
            }
            Printer.cmdGoHomeAll();//GCode.homeAxes();
            consoleMain->appendPlainText("Heights checked");
        }
        OpenDACT.heightsSet = false;
    }
    qDebug() << "    }//handleInput()";
}

void COpendactWnd::positionFlowOld()
{
    qDebug() << "      positionFlow(){";
    float probingHeight = UserVariables.probingHeight;
    float plateDiameter = UserVariables.plateDiameter;
    int pauseTimeSet = UserVariables.pauseTimeSet;
    float valueZ = 0.482F * plateDiameter;
    float valueXYLarge = 0.417F * plateDiameter;
    float valueXYSmall = 0.241F * plateDiameter;

    Q_UNUSED(pauseTimeSet);

    if (UserVariables.probeChoice == "Z-Probe" && wasSet == false){
        switch (iteration){
        case 0:
            qDebug() << "        handleInput() wasSet == false 0";
            EEPROM.zProbeHeight = 0;
            Printer.cmdGoHomeAll();
            iteration++;
            break;
        case 1:
            qDebug() << "        handleInput() wasSet == false 1";
            Printer.cmdGoToXYZ(0,0,int(EEPROM.zMaxLength/6.0+0.5));
            iteration++;
            break;
        case 2:
            qDebug() << "        handleInput() wasSet == false 1";
            Printer.cmdGetZProbeValue();// probe();
            wasSet = true;
//            setEEPROMGUIList();
//            EEPROM.sendEEPROM();
            iteration = 0;
            break;
        }
        /*
        pauseTimeZMax();
        pauseTimeZMax();
        pauseTimeProbe();
        */
    }
    else{
        Printer.sendCmd(QString("G0 F%1").arg(UserVariables.xySpeed * 60),false);//converts mm/s to mm/min
        switch (currentPosition){
        case 0:
            switch (iteration){
            case 0:
                qDebug() << "        handleInput() 0-0";
                Printer.cmdGoHomeAll();
                iteration++;
                break;
            case 1:
                qDebug() << "        handleInput() 0-1";
                Printer.cmdGoToXYZ(0,0,probingHeight);// "G1 Z" + probingHeight + " X0 Y0";
                iteration++;
                break;
            case 2:
                qDebug() << "        handleInput() 0-2";
                Printer.cmdGetZProbeValue();// probe();
                iteration++;
                break;
            case 3:
                //Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
                //                                 " X-" + valueXYLarge.ToString() +
                //                                 " Y-" + valueXYSmall.ToString());
                qDebug() << "        handleInput() 0-3";
                Printer.cmdGoToXYZ(-valueXYLarge,-valueXYSmall,probingHeight);
                currentPosition++;
                iteration = 0;
                break;
            }
            /*
            pauseTimeZMaxThird();
            pauseTimeZMaxThird();
            pauseTimeZMax();
            pauseTimeProbe();
            pauseTimeRadius();
            */
            break;
        case 1:
            switch (iteration){
            case 0:
                qDebug() << "        handleInput() 1-0";
                Printer.cmdGetZProbeValue();// probe();
                iteration++;
                break;
            case 1:
                qDebug() << "        handleInput() 1-1";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X-" + valueXYLarge.ToString() +
//                                                 " Y-" + valueXYSmall.ToString());
                Printer.cmdGoToXYZ(-valueXYLarge,-valueXYSmall,probingHeight);
                iteration++;
                break;
            case 2:
                qDebug() << "        handleInput() 1-2";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() + " X0 Y0");
                Printer.cmdGoToXYZ(0,0,probingHeight);
                iteration++;
                break;
            case 3:
                qDebug() << "        handleInput() 1-3";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X" + valueXYLarge.ToString() +
//                                                 " Y" + valueXYSmall.ToString());
                Printer.cmdGoToXYZ(valueXYLarge,valueXYSmall,probingHeight);
                currentPosition++;
                iteration = 0;
                break;
            }
            /*
            pauseTimeProbe();
            pauseTimeRadius();
            pauseTimeRadius();
            pauseTimeRadius();
            currentPosition++;
            */
            break;
        case 2:
            switch (iteration){
            case 0:
                qDebug() << "        handleInput() 2-0";
                Printer.cmdGetZProbeValue();// probe();
                iteration++;
                break;
            case 1:
                qDebug() << "        handleInput() 2-1";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X" + valueXYLarge.ToString() +
//                                                 " Y" + valueXYSmall.ToString());
                Printer.cmdGoToXYZ(valueXYLarge,valueXYSmall,probingHeight);
                iteration++;
                break;
            case 2:
                qDebug() << "        handleInput() 2-2";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() + " X0 Y0");
                Printer.cmdGoToXYZ(0,0,probingHeight);
                iteration++;
                break;
            case 3:
                qDebug() << "        handleInput() 2-3";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X" + valueXYLarge.ToString() +
//                                                 " Y-" + valueXYSmall.ToString());
                Printer.cmdGoToXYZ(valueXYLarge,-valueXYSmall,probingHeight);
                currentPosition++;
                iteration = 0;
                break;
            }
            /*
            pauseTimeProbe();
            pauseTimeRadius();
            pauseTimeRadius();
            pauseTimeRadius();
            currentPosition++;
            */
            break;
        case 3:
            switch (iteration){
            case 0:
                qDebug() << "        handleInput() 3-0";
                Printer.cmdGetZProbeValue();// probe();
                iteration++;
                break;
            case 1:
                qDebug() << "        handleInput() 3-1";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X" + valueXYLarge.ToString() +
//                                                 " Y-" + valueXYSmall.ToString());
                Printer.cmdGoToXYZ(valueXYLarge,-valueXYSmall,probingHeight);
                iteration++;
                break;
            case 2:
                qDebug() << "        handleInput() 3-2";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() + " X0 Y0");
                Printer.cmdGoToXYZ(0,0,probingHeight);
                iteration++;
                break;
            case 3:
                qDebug() << "        handleInput() 3-3";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X-" + valueXYLarge.ToString() +
//                                                 " Y" + valueXYSmall.ToString());
                Printer.cmdGoToXYZ(-valueXYLarge,valueXYSmall,probingHeight);
                currentPosition++;
                iteration = 0;
                break;
            }
            /*
            pauseTimeProbe();
            pauseTimeRadius();
            pauseTimeRadius();
            pauseTimeRadius();
            currentPosition++;
            */
            break;
        case 4:
            switch (iteration){
            case 0:
                qDebug() << "        handleInput() 4-0";
                Printer.cmdGetZProbeValue();// probe();
                iteration++;
                break;
            case 1:
                qDebug() << "        handleInput() 4-1";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X-" + valueXYLarge.ToString() +
//                                                 " Y" + valueXYSmall.ToString());
                Printer.cmdGoToXYZ(-valueXYLarge,valueXYSmall,probingHeight);
                iteration++;
                break;
            case 2:
                qDebug() << "        handleInput() 4-2";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() + " X0 Y0");
                Printer.cmdGoToXYZ(0,0,probingHeight);
                iteration++;
                break;
            case 3:
                qDebug() << "        handleInput() 4-3";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X0 Y" + valueZ.ToString());
                Printer.cmdGoToXYZ(0,valueZ,probingHeight);
                currentPosition++;
                iteration = 0;
                break;
            }
            /*
            pauseTimeProbe();
            pauseTimeRadius();
            pauseTimeRadius();
            pauseTimeRadius();
            currentPosition++;
            */
            break;
        case 5:
            switch (iteration){
            case 0:
                qDebug() << "        handleInput() 5-0";
                Printer.cmdGetZProbeValue();// probe();
                iteration++;
                break;
            case 1:
                qDebug() << "        handleInput() 5-1";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X0 Y" + valueZ.ToString());
                Printer.cmdGoToXYZ(0,valueZ,probingHeight);
                iteration++;
                break;
            case 2:
                qDebug() << "        handleInput() 5-2";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X0 Y-" + valueZ.ToString());
                Printer.cmdGoToXYZ(0,-valueZ,probingHeight);
                currentPosition++;
                iteration = 0;
                break;
            }
            /*
            pauseTimeProbe();
            pauseTimeRadius();
            pauseTimeRadius();
            currentPosition++;
            */
            break;
        case 6:
            switch (iteration){
            case 0:
                qDebug() << "        handleInput() 6-0";
                Printer.cmdGetZProbeValue();// probe();
                iteration++;
                break;
            case 1:
                qDebug() << "        handleInput() 6-1";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X0 Y-" + valueZ.ToString());
                Printer.cmdGoToXYZ(0,-valueZ,probingHeight);
                iteration++;
                break;
            case 2:
                qDebug() << "        handleInput() 6-2";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() + " X0 Y0");
                Printer.cmdGoToXYZ(0,0,probingHeight);
                iteration++;
                if (OpenDACT.calibrateInProgress == false){ iteration++; }
                break;
            case 3:
                qDebug() << "        handleInput() 6-3";
//                Connection._serialPort.WriteLine("G1 Z" + Convert.ToInt32(EEPROM.zMaxLength / 3) + " X0 Y0");
                Printer.cmdGoToXYZ(0,0,int(EEPROM.zMaxLength / 3+0.5));
                iteration++;
                break;
            case 4:
                qDebug() << "        handleInput() 6-4";
                currentPosition = 0;
                OpenDACT.checkHeights = false;
                iteration = 0;
                Printer.sendCmd(QString("G0 F%1").arg(UserVariables.xySpeed * 60),true);//converts mm/s to mm/min
                break;
            }
            /*
            pauseTimeProbe();
            pauseTimeRadius();
            pauseTimeRadius();
            pauseTimeZMaxThird();
            */
            break;
        }//end switch
    }//end else
    qDebug() << "      } //positionFlow()";
}

COpendactWnd::COpendactWnd(QWidget *parent) :
    CBaseWindow(parent),Ui::opendact_wnd()
{
    step  = NoStep;
    state = Stoped;

    setupUi(this);
    setWindowTitle("OpenDACT");
    setWindowIcon(QIcon(":/images/opendact.png"));
//    layout()->addWidget(new CHeightmapWidget());
    initUi();
    connect (&OpenDACT,SIGNAL(signalLogConsole(QString)),SLOT(slotLogConsole(QString)));
    connect (&OpenDACT,SIGNAL(signalLogConsole(QString,Qt::GlobalColor)),SLOT(slotLogConsole(QString,Qt::GlobalColor)));
    connect (&OpenDACT,SIGNAL(signalSetAccuracyPoint(int,float)),SLOT(slotSetAccuracyPoint(int,float)));
    connect (&Printer,SIGNAL(signalCommandExecuted()),this,SLOT(slotCommandExecuted()),Qt::QueuedConnection);
    connect (&Printer      ,SIGNAL(signalReady(QString)),SLOT(slotReady(QString)),Qt::QueuedConnection);
    connect (&Printer      ,SIGNAL(signalNewPosition(TVertex)),SLOT(slotNewPosition(TVertex)));
    connect (Printer.EEPROM,SIGNAL(signalReady(QString)),SLOT(slotReady(QString)),Qt::QueuedConnection);
    connect (this,SIGNAL(signalNextStep()),SLOT(slotCommandExecuted()),Qt::QueuedConnection);
    connect (InputHeightMap,SIGNAL(pushButtonClicked(CHeightmapWidget::EBushButtons)),SLOT(onInputPushButtonClicked(CHeightmapWidget::EBushButtons)));
    connect (autoCheckBox,SIGNAL(clicked(bool)),SLOT(on_autoCheckBox(bool)));
    iteration           = 0;
    wasSet              = false;
    currentPosition     = 0;
    checkHeightsOnly    = false;
    updateControls();
}

COpendactWnd::~COpendactWnd()
{
}

void COpendactWnd::initUi()
{
    InputHeightMap    = new CHeightmapWidget();
    InputHeightMap->SetName("Input:");
    InputHeightMap->SetHeights(-0.2,0.06,0.06,-0.41,-0.14,0.03);
    InputHeightMap->setEditable(true);
    InputHeightMap->setShowButtons(true);

    ExpectedHeightMap = new CHeightmapWidget();
    ExpectedHeightMap->SetName("Expected:");
    ExpectedHeightMap->SetHeights(0,0,0,0,0,0);

    autoCheckBox = new QCheckBox();
    autoCheckBox->setText("Automatic read printer value.");

    verticalLayout_5->addWidget(autoCheckBox);
    verticalLayout_5->addWidget(InputHeightMap);
    verticalLayout_5->addWidget(ExpectedHeightMap);


    FirstHeightMap   = new CHeightmapWidget();
    CurrentHeightMap = new CHeightmapWidget();
    accuracyTime     = new C2dChart();
    //    0   0.0366669
    //    1   0.0250092
//    0   0.0299988
//    1   0.0316518

//    accuracyTime->append(TIntFloatPoint(0,0.0));
//    accuracyTime->append(TIntFloatPoint(1,0.026));
//    accuracyTime->append(TIntFloatPoint(2,0.030));

//    accuracyTime->append(TIntFloatPoint(3,2));
//    accuracyTime->append(TIntFloatPoint(2,3.5));
//    accuracyTime->append(TIntFloatPoint(4,1.5));
//    accuracyTime->append(TIntFloatPoint(5,1.2));
//    accuracyTime->append(TIntFloatPoint(6,1.2));
//    accuracyTime->append(TIntFloatPoint(7,1.2));
//    accuracyTime->append(TIntFloatPoint(8,1.2));
//    accuracyTime->append(TIntFloatPoint(9,1.2));
////    accuracyTime->append(TIntFloatPoint(10,1.2));
////    accuracyTime->append(TIntFloatPoint(11,1.2));
////    accuracyTime->append(TIntFloatPoint(12,1.2));

    heightMapsLayout->addWidget(FirstHeightMap);
    heightMapsLayout->addWidget(CurrentHeightMap);

    FirstHeightMap->SetName("First Height-Map:");
    FirstHeightMap->SetHeights(0,0,0,0,0,0);
    CurrentHeightMap->SetName("Current Height-Map:");
    CurrentHeightMap->SetHeights(0,0,0,0,0,0);
    resetPrinter->setVisible(false);
    tabControl1->setVisible(false);

    accuracyTimeHolder->addWidget(accuracyTime);

    // Дефолтные значения
    textPlateDiameter    ->setValue(140);
    diagonalRodLengthText->setValue(217);
    QStringList ls; ls << "FSR" << "Z-Probe";
    comboBoxZMin    ->addItems(ls);
    comboBoxZMin    ->setCurrentText("Z-Probe");

    textAccuracy         ->setValue(0.01);
    textAccuracy2        ->setValue(0.035);
    textHRadRatio        ->setValue(-0.5);
    textFSRPO            ->setValue(0.4);
    textMaxIterations    ->setValue(50);
    textProbingSpeed     ->setValue(5);
    textProbingHeight    ->setValue(20);
    ls.clear(); ls << "True" << "False";
    heuristicComboBox    ->addItems(ls);
    heuristicComboBox    ->setCurrentText("False");
    heuristicComboBox    ->setEnabled(false);

    textDRadRatio        ->setValue(-0.5);
    textDeltaTower       ->setValue(0.13083);
    textDeltaOpp         ->setValue(0.21083);
    alphaText            ->setValue(2.5 );
    textOffsetPerc       ->setValue(-0.6);
    textMainOppPerc      ->setValue(0.5 );
    textTowPerc          ->setValue(0.3 );
    textOppPerc          ->setValue(-0.25);
    xySpeedTxt           ->setValue(100 );
    textBox6             ->setValue(1.5 );
    textBox5             ->setValue(0.5 );
    textBox4             ->setValue(0.25);
    textBox3             ->setValue(0.25);
    textBox2             ->setValue(0.25);
    textBox1             ->setValue(0.25);
}

void COpendactWnd::setButtonValues()
{
        textAccuracy         ->setValue(UserVariables.calculationAccuracy);
        textAccuracy2        ->setValue(UserVariables.accuracy);
        textHRadRatio        ->setValue(UserVariables.HRadRatio);
        textDRadRatio        ->setValue(UserVariables.DRadRatio);

        if (UserVariables.advancedCalibration) heuristicComboBox->setCurrentText("True");
        else                                   heuristicComboBox->setCurrentText("False");

//        textPauseTimeSet     ->setValue(OpenDACT.UserVariables.pauseTimeSet);
        textMaxIterations    ->setValue(UserVariables.maxIterations);
        textProbingSpeed     ->setValue(UserVariables.probingSpeed);
        textFSRPO            ->setValue(UserVariables.FSROffset);
        textDeltaOpp         ->setValue(UserVariables.deltaOpp);
        textDeltaTower       ->setValue(UserVariables.deltaTower);
        diagonalRodLengthText->setValue(UserVariables.diagonalRodLength);
        alphaText            ->setValue(UserVariables.alphaRotationPercentage);
        textPlateDiameter    ->setValue(UserVariables.plateDiameter);
        textProbingHeight    ->setValue(UserVariables.probingHeight);

        //XYZ Offset percs
        textOffsetPerc       ->setValue(UserVariables.offsetCorrection);
        textMainOppPerc      ->setValue(UserVariables.mainOppPerc);
        textTowPerc          ->setValue(UserVariables.towPerc);
        textOppPerc          ->setValue(UserVariables.oppPerc);
}

void COpendactWnd::setEEPROMGUIList()
{
        stepsPerMMText ->setValue(EEPROM.stepsPerMM);
        zMaxLengthText ->setValue(EEPROM.zMaxLength);
        zProbeText     ->setValue(EEPROM.zProbeHeight);
        zProbeSpeedText->setValue(EEPROM.zProbeSpeed);
        diagonalRod    ->setValue(EEPROM.diagonalRod);
        HRadiusText    ->setValue(EEPROM.HRadius);
        offsetXText    ->setValue(EEPROM.offsetX);
        offsetYText    ->setValue(EEPROM.offsetY);
        offsetZText    ->setValue(EEPROM.offsetZ);
        AText          ->setValue(EEPROM.A);
        BText          ->setValue(EEPROM.B);
        CText          ->setValue(EEPROM.C);
        DAText         ->setValue(EEPROM.DA);
        DBText         ->setValue(EEPROM.DB);
        DCText         ->setValue(EEPROM.DC);
}

void COpendactWnd::setUserVariables()
{
    UserVariables.calculationAccuracy = textAccuracy->value();
    UserVariables.accuracy            = textAccuracy2->value();
    UserVariables.HRadRatio           = textHRadRatio->value();
    UserVariables.DRadRatio           = textDRadRatio->value();

    UserVariables.probeChoice         = comboBoxZMin->currentText();
    if (heuristicComboBox->currentText().toLower()=="true")
        UserVariables.advancedCalibration = true;
    else
        UserVariables.advancedCalibration = false;

//    UserVariables.pauseTimeSet = textPauseTimeSet->value();
    UserVariables.maxIterations           = textMaxIterations->value();
    UserVariables.probingSpeed            = textProbingSpeed->value();
    UserVariables.FSROffset               = textFSRPO->value();
    UserVariables.deltaOpp                = textDeltaOpp->value();
    UserVariables.deltaTower              = textDeltaTower->value();
    UserVariables.diagonalRodLength       = diagonalRodLengthText->value();
    UserVariables.alphaRotationPercentage = alphaText->value();
    UserVariables.plateDiameter           = textPlateDiameter->value();
    UserVariables.probingHeight           = textProbingHeight->value();

    //XYZ Offset percs
    UserVariables.offsetCorrection = textOffsetPerc->value();
    UserVariables.mainOppPerc      = textMainOppPerc->value();
    UserVariables.towPerc          = textTowPerc->value();
    UserVariables.oppPerc          = textOppPerc->value();

    UserVariables.xySpeed          = xySpeedTxt->value();
}

void COpendactWnd::on_openAdvanced_clicked()
{
    tabControl1->setVisible(!tabControl1->isVisible());
}

void COpendactWnd::slotLogConsole(const QString& str)
{
    QTextCharFormat fmt = consoleMain->currentCharFormat();
    fmt.clearForeground();
    consoleMain->setCurrentCharFormat(fmt);
    consoleMain->appendPlainText(str);
}

void COpendactWnd::slotLogConsole (const QString& str, Qt::GlobalColor color)
{
    QTextCharFormat fmt = consoleMain->currentCharFormat();
    fmt.setForeground(QBrush(color));
    consoleMain->setCurrentCharFormat(fmt);
    consoleMain->appendPlainText(str);
}


void COpendactWnd::slotSetAccuracyPoint(int iteration_num, float temp_accuracy)
{
//    Q_UNUSED(iteration_num)
//    Q_UNUSED(temp_accuracy)

    accuracyTime->append(TIntFloatPoint(iteration_num,temp_accuracy));
    qDebug() << iteration_num << " " << temp_accuracy;
}


void COpendactWnd::setHeightsInvoke()
{
    if (OpenDACT.iterationNum==0){
        FirstHeightMap->SetHeights(Heights.X,Heights.XOpp,
                                   Heights.Y,Heights.YOpp,
                                   Heights.Z,Heights.ZOpp);
    }
    CurrentHeightMap->SetHeights(Heights.X,Heights.XOpp,
                               Heights.Y,Heights.YOpp,
                               Heights.Z,Heights.ZOpp);
}


void COpendactWnd::slotCommandExecuted ()
{
    mainLoop();
    //OpenDACT.handleInput();
}

void COpendactWnd::slotReady (const QString&)
{
    mainLoop();
    //OpenDACT.handleInput();
}

void COpendactWnd::updateControls()
{
    bool main,bstart,bstop;
    bstart = true;
    bstop  = true;
    main   = false;
    QString str;
    if (checkHeightsOnly) str = tr("Check Current Heights");
    else                  str = tr("Calibrate");

    switch (state) {
    case Stoped:
        main  = true;
        bstop = false;
        break;
    case Active:
        str = tr("Pause");
        break;
    case Paused:
        str = tr("Resume");
        break;
    case WaitPaused:
        str = tr("Pause");
        bstop = false;
        bstart = false;
        break;
    case WaitStop:
        str = tr("Pause");
        bstop = false;
        bstart = false;
        break;
    default:
        main = false;
        break;
    }

    textPlateDiameter->setEnabled(main);
    diagonalRodLengthText->setEnabled(main);
    comboBoxZMin->setEnabled(main);
    tab->setEnabled(main);

    if (!checkHeightsOnly) {
        calibrateButton->setEnabled (bstart);
        calibrateButton->setText    (str);
        checkHeightsBut->setEnabled(main);
        checkHeightsBut->setText("Check Current Heights");
    }
    else{
        checkHeightsBut ->setEnabled(bstart);
        checkHeightsBut ->setText   (str);
        calibrateButton->setEnabled(main);
        calibrateButton->setText("Calibrate");
    }
    stopBut        ->setEnabled (bstop);
}

void COpendactWnd::on_stopBut_clicked()
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

void COpendactWnd::on_checkHeightsBut_clicked()
{
    if (state==Stoped){
        checkHeightsOnly = true;
        start();
        setUserVariables();
        state= Active;
        step = CheckHieghts;
        iteration       = 0;
        currentPosition = 0;
        OpenDACT.iterationNum = 0;
        Heights.init();
        updateControls();
        emit signalNextStep();
    }
    else if (state==Active){
        state= WaitPaused;
    }
    else if (state==Paused){
        state= Active;
        mainLoop();
    }
    updateControls();
}

void COpendactWnd::on_readEepromMan_clicked()
{
    EEPROM.readEEPROM();
    spmMan    ->setValue(EEPROM.stepsPerMM);
    zMaxMan   ->setValue(EEPROM.zMaxLength);
    zProHeiMan->setValue(EEPROM.zProbeHeight);
    zProSpeMan->setValue(EEPROM.zProbeSpeed);
    horRadMan ->setValue(EEPROM.HRadius);
    diaRodMan ->setValue(EEPROM.diagonalRod);
    towOffXMan->setValue(EEPROM.offsetX);
    towOffYMan->setValue(EEPROM.offsetY);
    towOffZMan->setValue(EEPROM.offsetZ);
    alpRotAMan->setValue(EEPROM.A);
    alpRotBMan->setValue(EEPROM.B);
    alpRotCMan->setValue(EEPROM.C);
    delRadAMan->setValue(EEPROM.DA);
    delRadBMan->setValue(EEPROM.DB);
    delRadCMan->setValue(EEPROM.DC);
}

void COpendactWnd::on_manualCalibrateBut_clicked()
{
    slotLogConsole("Manual calibration.",Qt::blue);
    setUserVariables();
    Heights.X   =InputHeightMap->getX();
    Heights.XOpp=InputHeightMap->getXOpp();
    Heights.Y   =InputHeightMap->getY();
    Heights.YOpp=InputHeightMap->getYOpp();
    Heights.Z   =InputHeightMap->getZ();
    Heights.ZOpp=InputHeightMap->getZOpp();

    EEPROM.stepsPerMM   = spmMan->value();
    EEPROM.tempSPM      = spmMan->value();
    EEPROM.zMaxLength   = zMaxMan->value();
    EEPROM.zProbeHeight = zProHeiMan->value();
    EEPROM.zProbeSpeed  = zProSpeMan->value();
    EEPROM.HRadius      = horRadMan->value();
    EEPROM.diagonalRod  = diaRodMan->value();
    EEPROM.offsetX      = towOffXMan->value();
    EEPROM.offsetY      = towOffYMan->value();
    EEPROM.offsetZ      = towOffZMan->value();
    EEPROM.A            = alpRotAMan->value();
    EEPROM.B            = alpRotBMan->value();
    EEPROM.C            = alpRotCMan->value();
    EEPROM.DA           = delRadAMan->value();
    EEPROM.DB           = delRadBMan->value();
    EEPROM.DC           = delRadCMan->value();
    OpenDACT.basicCalibration();

    //set eeprom vals in manual calibration
    spmMan    ->setValue(EEPROM.stepsPerMM);
    zMaxMan   ->setValue(EEPROM.zMaxLength);
    zProHeiMan->setValue(EEPROM.zProbeHeight);
    zProSpeMan->setValue(EEPROM.zProbeSpeed);
    diaRodMan ->setValue(EEPROM.diagonalRod);
    horRadMan ->setValue(EEPROM.HRadius);
    towOffXMan->setValue(EEPROM.offsetX);
    towOffYMan->setValue(EEPROM.offsetY);
    towOffZMan->setValue(EEPROM.offsetZ);
    alpRotAMan->setValue(EEPROM.A);
    alpRotBMan->setValue(EEPROM.B);
    alpRotCMan->setValue(EEPROM.C);
    delRadAMan->setValue(EEPROM.DA);
    delRadBMan->setValue(EEPROM.DB);
    delRadCMan->setValue(EEPROM.DC);

    //set expected height map
    ExpectedHeightMap->SetHeights(Heights.X,Heights.XOpp,Heights.Y,Heights.YOpp,Heights.Z,Heights.ZOpp);
//    this.xExp.Text = Heights.X.ToString();
//    this.xOppExp.Text = Heights.XOpp.ToString();
//    this.yExp.Text = Heights.Y.ToString();
//    this.yOppExp.Text = Heights.YOpp.ToString();
//    this.zExp.Text = Heights.Z.ToString();
//    this.zOppExp.Text = Heights.ZOpp.ToString();

}

void COpendactWnd::onInputPushButtonClicked(CHeightmapWidget::EBushButtons but)
{
    float plateDiameter = textPlateDiameter->value();
    float probingHeight = InputHeightMap->getZOffset();
    float valueXYLarge = 0.417F * plateDiameter;
    float valueXYSmall = 0.241F * plateDiameter;
    float valueZ = 0.482F * plateDiameter;
//    qDebug() << but;
    switch (but) {
    case CHeightmapWidget::EHomeAll:
        Printer.cmdGoHomeAll();
        break;
    case CHeightmapWidget::EX:
        Printer.cmdGoToXYZ(-valueXYLarge,-valueXYSmall,probingHeight);
        break;
    case CHeightmapWidget::EXOpp:
        Printer.cmdGoToXYZ(valueXYLarge,valueXYSmall,probingHeight);
        break;
    case CHeightmapWidget::EY:
        Printer.cmdGoToXYZ(valueXYLarge,-valueXYSmall,probingHeight);
        break;
    case CHeightmapWidget::EYOpp:
        Printer.cmdGoToXYZ(-valueXYLarge,valueXYSmall,probingHeight);
        break;
    case CHeightmapWidget::EZ:
        Printer.cmdGoToXYZ(0,valueZ,probingHeight);
        break;
    case CHeightmapWidget::EZOpp:
        Printer.cmdGoToXYZ(0,-valueZ,probingHeight);
        break;
    default:
        break;
    }
}

void COpendactWnd::on_sendEepromMan_clicked()
{
    EEPROM.sendEEPROM();
}

void COpendactWnd::on_autoCheckBox(bool val)
{
    InputHeightMap->setAutoReadValues(val);
}

void COpendactWnd::slotNewPosition(const TVertex& ver)
{
//    qDebug() << "slotNewPosition(const TVertex& ver)";
    InputHeightMap->on_NewValue(ver.Z);
}

