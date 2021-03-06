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
        Printer.sendGoHomeAll();
        iteration++;
        break;
    case 2:
        Printer.sendGoToXYZ(0,0,int(EEPROM.zMaxLength/6.0+0.5));
        iteration++;
        break;
    case 3:
        Printer.sendGetZProbeValue();// probe();
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
            Printer.sendGoHomeAll();
            iteration++;
            break;
        case 1:
            //qDebug() << "        handleInput() 0-1";
            Printer.sendGoToXYZ(0,0,probingHeight);// "G1 Z" + probingHeight + " X0 Y0";
            iteration++;
            break;
        case 2:
            //qDebug() << "        handleInput() 0-2";
            Printer.sendGetZProbeValue();// probe();
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
            Printer.sendGoToXYZ(-valueXYLarge,-valueXYSmall,probingHeight);
            currentPosition++;
            iteration = 0;
        }
        break;
    case 1:
        switch (iteration){
        case 0:
            //qDebug() << "        handleInput() 1-0";
            Printer.sendGetZProbeValue();// probe();
            iteration++;
            break;
        case 1:
            //qDebug() << "        handleInput() 1-1";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X-" + valueXYLarge.ToString() +
//                                                 " Y-" + valueXYSmall.ToString());
            Heights.setHeights(Printer.ZProbe.Z);
            setHeightsInvoke();
            Printer.sendGoToXYZ(-valueXYLarge,-valueXYSmall,probingHeight);
            iteration++;
            break;
        case 2:
            //qDebug() << "        handleInput() 1-2";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() + " X0 Y0");
            Printer.sendGoToXYZ(0,0,probingHeight);
            iteration++;
            break;
        case 3:
            //qDebug() << "        handleInput() 1-3";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X" + valueXYLarge.ToString() +
//                                                 " Y" + valueXYSmall.ToString());
            Printer.sendGoToXYZ(valueXYLarge,valueXYSmall,probingHeight);
            currentPosition++;
            iteration = 0;
            break;
        }
        break;
    case 2:
        switch (iteration){
        case 0:
            //qDebug() << "        handleInput() 2-0";
            Printer.sendGetZProbeValue();// probe();
            iteration++;
            break;
        case 1:
            //qDebug() << "        handleInput() 2-1";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X" + valueXYLarge.ToString() +
//                                                 " Y" + valueXYSmall.ToString());
            Heights.setHeights(Printer.ZProbe.Z);
            setHeightsInvoke();
            Printer.sendGoToXYZ(valueXYLarge,valueXYSmall,probingHeight);
            iteration++;
            break;
        case 2:
            //qDebug() << "        handleInput() 2-2";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() + " X0 Y0");
            Printer.sendGoToXYZ(0,0,probingHeight);
            iteration++;
            break;
        case 3:
            //qDebug() << "        handleInput() 2-3";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X" + valueXYLarge.ToString() +
//                                                 " Y-" + valueXYSmall.ToString());
            Printer.sendGoToXYZ(valueXYLarge,-valueXYSmall,probingHeight);
            currentPosition++;
            iteration = 0;
            break;
        }
        break;
    case 3:
        switch (iteration){
        case 0:
            //qDebug() << "        handleInput() 3-0";
            Printer.sendGetZProbeValue();// probe();
            iteration++;
            break;
        case 1:
            //qDebug() << "        handleInput() 3-1";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X" + valueXYLarge.ToString() +
//                                                 " Y-" + valueXYSmall.ToString());
            Heights.setHeights(Printer.ZProbe.Z);
            setHeightsInvoke();
            Printer.sendGoToXYZ(valueXYLarge,-valueXYSmall,probingHeight);
            iteration++;
            break;
        case 2:
            //qDebug() << "        handleInput() 3-2";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() + " X0 Y0");
            Printer.sendGoToXYZ(0,0,probingHeight);
            iteration++;
            break;
        case 3:
            //qDebug() << "        handleInput() 3-3";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X-" + valueXYLarge.ToString() +
//                                                 " Y" + valueXYSmall.ToString());
            Printer.sendGoToXYZ(-valueXYLarge,valueXYSmall,probingHeight);
            currentPosition++;
            iteration = 0;
            break;
        }
        break;
    case 4:
        switch (iteration){
        case 0:
            //qDebug() << "        handleInput() 4-0";
            Printer.sendGetZProbeValue();// probe();
            iteration++;
            break;
        case 1:
            //qDebug() << "        handleInput() 4-1";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X-" + valueXYLarge.ToString() +
//                                                 " Y" + valueXYSmall.ToString());
            Heights.setHeights(Printer.ZProbe.Z);
            setHeightsInvoke();
            Printer.sendGoToXYZ(-valueXYLarge,valueXYSmall,probingHeight);
            iteration++;
            break;
        case 2:
            //qDebug() << "        handleInput() 4-2";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() + " X0 Y0");
            Printer.sendGoToXYZ(0,0,probingHeight);
            iteration++;
            break;
        case 3:
            //qDebug() << "        handleInput() 4-3";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X0 Y" + valueZ.ToString());
            Printer.sendGoToXYZ(0,valueZ,probingHeight);
            currentPosition++;
            iteration = 0;
            break;
        }
        break;
    case 5:
        switch (iteration){
        case 0:
            //qDebug() << "        handleInput() 5-0";
            Printer.sendGetZProbeValue();// probe();
            iteration++;
            break;
        case 1:
            //qDebug() << "        handleInput() 5-1";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X0 Y" + valueZ.ToString());
            Heights.setHeights(Printer.ZProbe.Z);
            setHeightsInvoke();
            Printer.sendGoToXYZ(0,valueZ,probingHeight);
            iteration++;
            break;
        case 2:
            //qDebug() << "        handleInput() 5-2";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X0 Y-" + valueZ.ToString());
            Printer.sendGoToXYZ(0,-valueZ,probingHeight);
            currentPosition++;
            iteration = 0;
            break;
        }
        break;
    case 6:
        switch (iteration){
        case 0:
            //qDebug() << "        handleInput() 6-0";
            Printer.sendGetZProbeValue();// probe();
            iteration++;
            break;
        case 1:
            //qDebug() << "        handleInput() 6-1";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() +
//                                                 " X0 Y-" + valueZ.ToString());
            Heights.setHeights(Printer.ZProbe.Z);
            setHeightsInvoke();
            Printer.sendGoToXYZ(0,-valueZ,probingHeight);
            iteration++;
            break;
        case 2:
            //qDebug() << "        handleInput() 6-2";
//                Connection._serialPort.WriteLine("G1 Z" + probingHeight.ToString() + " X0 Y0");
            Printer.sendGoToXYZ(0,0,probingHeight);
            iteration++;
            if (OpenDACT.calibrateInProgress == false){ iteration++; }
            break;
        case 3:
            //qDebug() << "        handleInput() 6-3";
//                Connection._serialPort.WriteLine("G1 Z" + Convert.ToInt32(EEPROM.zMaxLength / 3) + " X0 Y0");
            Printer.sendGoToXYZ(0,0,int(EEPROM.zMaxLength / 3+0.5));
            iteration++;
            Heights.printHeights();
            setHeightsInvoke();
            if (checkHeightsOnly){
                step = GoHome;
                Printer.sendGoHomeAll();
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
                Printer.sendGoHomeAll();
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
        Printer.sendGoHomeAll();
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
    connect (&Printer,SIGNAL(signalCommandReady(int)),this,SLOT(slotCommandExecuted(int)),Qt::QueuedConnection);
    connect (&Printer      ,SIGNAL(signalNewPositionReady(TVertex)),SLOT(slotNewPosition(TVertex)));
    connect (&Printer      ,SIGNAL(signalReady(QString)),SLOT(slotReady(QString)),Qt::QueuedConnection);
    connect (this,SIGNAL(signalNextStep()),SLOT(slotCommandExecuted()),Qt::QueuedConnection);
    connect (InputHeightMap,SIGNAL(pushButtonClicked(CManualHeightmapWidget::EBushButtons)),
             SLOT(onInputPushButtonClicked(CManualHeightmapWidget::EBushButtons)));
    connect (autoCheckBox,SIGNAL(clicked(bool)),SLOT(on_autoCheckBox(bool)));
    iteration           = 0;
    currentPosition     = 0;
    checkHeightsOnly    = false;
    updateControls();

    //--    connect (Printer.EEPROM,SIGNAL(signalReady(QString)),SLOT(slotReady(QString)),Qt::QueuedConnection);
}

COpendactWnd::~COpendactWnd()
{
}

void COpendactWnd::initUi()
{
    InputHeightMap    = new CManualHeightmapWidget();
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
    comboBoxZMin    ->setEnabled(false);

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
    tabControl1->setCurrentIndex(0);
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

void COpendactWnd::getEEPROMGUIList()
{
        EEPROM.stepsPerMM   = stepsPerMMText ->value();
        EEPROM.zMaxLength   = zMaxLengthText ->value();
        EEPROM.zProbeHeight = zProbeText     ->value();
        EEPROM.zProbeSpeed  = zProbeSpeedText->value();
        EEPROM.diagonalRod  = diagonalRod    ->value();
        EEPROM.HRadius      = HRadiusText    ->value();
        EEPROM.offsetX      = offsetXText    ->value();
        EEPROM.offsetY      = offsetYText    ->value();
        EEPROM.offsetZ      = offsetZText    ->value();
        EEPROM.A            = AText          ->value();
        EEPROM.B            = BText          ->value();
        EEPROM.C            = CText          ->value();
        EEPROM.DA           = DAText         ->value();
        EEPROM.DB           = DBText         ->value();
        EEPROM.DC           = DCText         ->value();
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
    accuracyTime->append(TIntFloatPoint(iteration_num,temp_accuracy));
//    qDebug() << iteration_num << " " << temp_accuracy;
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


void COpendactWnd::slotCommandExecuted (int cmd)
{
    Q_UNUSED(cmd);
    mainLoop();
}

void COpendactWnd::slotReady (const QString&)
{
    mainLoop();
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
//    comboBoxZMin->setEnabled(main);
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

void COpendactWnd::getManualEEPROMGUIList()
{
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

    getManualEEPROMGUIList();
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

    ExpectedHeightMap->SetHeights(Heights.X,Heights.XOpp,Heights.Y,Heights.YOpp,Heights.Z,Heights.ZOpp);
}

void COpendactWnd::onInputPushButtonClicked(CManualHeightmapWidget::EBushButtons but)
{
    float plateDiameter = textPlateDiameter->value();
    float probingHeight = InputHeightMap->getZOffset();
    float valueXYLarge = 0.417F * plateDiameter;
    float valueXYSmall = 0.241F * plateDiameter;
    float valueZ = 0.482F * plateDiameter;
//    qDebug() << but;
    switch (but) {
    case CManualHeightmapWidget::EHomeAll:
        Printer.sendGoHomeAll();
        break;
    case CManualHeightmapWidget::EX:
        Printer.sendGoToXYZ(-valueXYLarge,-valueXYSmall,probingHeight);
        break;
    case CManualHeightmapWidget::EXOpp:
        Printer.sendGoToXYZ(valueXYLarge,valueXYSmall,probingHeight);
        break;
    case CManualHeightmapWidget::EY:
        Printer.sendGoToXYZ(valueXYLarge,-valueXYSmall,probingHeight);
        break;
    case CManualHeightmapWidget::EYOpp:
        Printer.sendGoToXYZ(-valueXYLarge,valueXYSmall,probingHeight);
        break;
    case CManualHeightmapWidget::EZ:
        Printer.sendGoToXYZ(0,valueZ,probingHeight);
        break;
    case CManualHeightmapWidget::EZOpp:
        Printer.sendGoToXYZ(0,-valueZ,probingHeight);
        break;
    default:
        break;
    }
}

void COpendactWnd::on_sendEepromMan_clicked()
{
    getManualEEPROMGUIList();
    EEPROM.sendEEPROM();
}

void COpendactWnd::on_autoCheckBox(bool val)
{
    InputHeightMap->setAutoReadValues(val);
}

void COpendactWnd::slotNewPosition(const TVertex& ver)
{
    InputHeightMap->on_NewValue(ver.Z);
}


void COpendactWnd::on_readEEPROM_clicked()
{
    EEPROM.readEEPROM();
    setEEPROMGUIList();
}

void COpendactWnd::on_sendEEPROMButton_clicked()
{
    getEEPROMGUIList();
    EEPROM.sendEEPROM();
}
