#include "escher3d_wnd.h"
#include <math.h>
#include <QDebug>
#include "../../common.h"

extern double fsquare(double x);
/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
CEscher3dWindow::CEscher3dWindow(QWidget *parent) :
    CBaseWindow(parent),Ui::escher3d_wnd()
{
    setupUi(this);

    TableVertex = new QTableVertex();
    groupBox_2->layout()->addWidget(TableVertex);
    connect(TableVertex,SIGNAL(signalBPCliked    (int)),SLOT(slotBPCliked    (int)));
    connect(TableVertex,SIGNAL(signalValueChanged(int)),SLOT(slotValueChanged(int)));

    setWindowTitle("escher3d.com");
    setWindowIcon(QIcon(":/images/esher3d.png"));
    stepspermm->setValue(100);
    oldxstop->setValue(0);//51);
    oldystop->setValue(0);
    oldzstop->setValue(0);//107);
    oldrodlength->setValue(217);
    oldradius->setValue(95);
    oldhomedheight->setValue(300);//297.25);
    oldxpos->setValue(0);
    oldypos->setValue(0);
    oldzpos->setValue(0);
    bedradius->setValue(75);
    numPoints->setValue(10);
    oldNumPoints = 10;
//    numfactors->addItem("3");
    numfactors->addItem("4");
    numfactors->addItem("6");
    numfactors->addItem("7");
    numfactors->setCurrentText("6");
    copyButton->setDisabled(true);
    dsbHeight->setValue(20);
    currectVertexIndex = -1;
    connect (&Printer,SIGNAL(signalCommandReady(int)),this,SLOT(slotCommandExecuted(int)),Qt::QueuedConnection);
    pbRecalc->setEnabled(false);
    updateControls();
    calcProbePoints();
    on_cbAutoManual_clicked();
    connect (&Printer,SIGNAL(signalNewPositionReady(TVertex)),SLOT(slotNewPositionReady(TVertex)));
}

CEscher3dWindow::~CEscher3dWindow()
{
}

void CEscher3dWindow::slotCommandExecuted(int)
{
    mainLoop();
}

void CEscher3dWindow::on_pbStart_clicked()
{
    copyButton->setDisabled(true);
    if (state==Stoped){
        start();
        calcProbePoints();
//        pbRecalc->setEnabled(false);
        state= Active;
        step = GoHome;
        currectVertexIndex = 0;
        Printer.sendGoHomeAll();
        updateControls();
    }
}

void CEscher3dWindow::mainLoop()
{
//    qDebug() << "CEscher3dWindow::slotCommandExecuted(int)" << state  << step;
    switch (state) {
    case Active:
        activeLoop();
        break;
    default:
        break;
    }
    updateControls();
}

void CEscher3dWindow::activeLoop()
{
    double z;
    switch (step){
    case GoHome:
        currectVertexIndex = 0;
        if (!gotoxyz()){
            state = Stoped;
            stop();
        }
        break;
    case GoToXYZ:
        Printer.sendGetZProbeValue();
        step = ZProbe;
        break;
    case ZProbe:
        z = Printer.ZProbe.Z;
        if (!qIsNaN(z)) z = z - dsbHeight->value();//+ dsbProbeHeight->value() -dsbHeight->value();
        TableVertex->setZ(currectVertexIndex,z);
        currectVertexIndex++;
        if (!gotoxyz()){
            state = Stoped;
            calc();
            pbRecalc->setEnabled(true);
            stop();
        }
        break;
    default:
        break;
    }
}

bool CEscher3dWindow::gotoxyz ()
{
    int count = TableVertex->getSize();
    if (count>currectVertexIndex){
        step = GoToXYZ;
        Printer.sendGoToXYZ(TableVertex->getX(currectVertexIndex),
                           TableVertex->getY(currectVertexIndex),
                           dsbHeight->value());
        return true;
    }
    return false;
}


void CEscher3dWindow::on_pbRecalc_clicked()
{
    calc();
    updateControls();
}

void CEscher3dWindow::on_pushButton_clicked()
{
    CEePromRecord* record;
    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Tower X endstop offset"));
    if (record) oldxstop->setValue(record->IValue);
    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Tower Y endstop offset"));
    if (record) oldystop->setValue(record->IValue);
    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Tower Z endstop offset"));
    if (record) oldzstop->setValue(record->IValue);

    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Diagonal rod length"));
    if (record) oldrodlength->setValue(record->FValue);
    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Horizontal rod radius at 0,0"));
    if (record) oldradius->setValue(record->FValue);
    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Z max length"));
    if (record) oldhomedheight->setValue(record->FValue);

    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Alpha A(210)"));
    if (record) oldxpos->setValue(record->FValue-210.0);
    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Alpha B(330)"));
    if (record) oldypos->setValue(record->FValue-330.0);
    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Alpha C(90)"));
    if (record) oldzpos->setValue(record->FValue-90.0);

    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Steps per mm"));
    if (record) stepspermm->setValue(record->FValue);
}

void CEscher3dWindow::on_cbAutoManual_clicked()
{
    bool fl = cbAutoManual->isChecked();
    pbStart       ->setVisible(fl);
    autoCheckBox  ->setVisible(!fl);
    homeAllButton ->setVisible(!fl);
    TableVertex   ->setButtonVisible(!fl);
    TableVertex   ->setEditable(!fl);
    clearAllValues->setVisible(!fl);
    updateControls();
}

void CEscher3dWindow::on_autoCheckBox_clicked()
{
    TableVertex->setButtonCheckable(autoCheckBox->isChecked());
    currectVertexIndex = -1;
    updateControls();
}

void CEscher3dWindow::on_homeAllButton_clicked()
{
    TableVertex->uncheckButtons();
    Printer.sendGoHomeAll();
    currectVertexIndex = -1;
    updateControls();
}

void CEscher3dWindow::slotBPCliked(int code)
{
    currectVertexIndex = code;
    Printer.sendGoToXYZ(TableVertex->getX(currectVertexIndex),
                       TableVertex->getY(currectVertexIndex),
                       dsbHeight->value());
    updateControls();
}

void CEscher3dWindow::slotValueChanged(int code)
{
    Q_UNUSED(code);
    updateControls();
}

void CEscher3dWindow::slotNewPositionReady(const TVertex &ver)
{
    if (state!=Active)
        TableVertex->setZ(currectVertexIndex,-ver.Z);
    updateControls();
}

void CEscher3dWindow::on_clearAllValues_clicked()
{
//    calcProbePoints();
    for (int i=0,n=TableVertex->getSize();i<n;i++)
        TableVertex->setZ(i,qQNaN());
    updateControls();
}

// ---                           escher3d.com                           --- //
void CEscher3dWindow::calcProbePoints()
{
    int    num_Points = numPoints->value();
    double bed_Radius = bedradius->value();
    TableVertex->setSize(num_Points);
    if (num_Points==4) {
        for (int i=0;i<3;i++) {
            TableVertex->set(i,bed_Radius*sin(2.0*M_PI*i/3.0),bed_Radius*cos(2.0*M_PI*i/3.0));
        }
        TableVertex->set(3,0,0);
    }
    else {
        if (num_Points>=7) {
            for (int i=0;i<6;i++) {
                TableVertex->set(i,bed_Radius*sin(2.0*M_PI*i/6.0),bed_Radius*cos(2.0*M_PI*i/6.0));
            }
        }
        if (num_Points>=10) {
            for (int i=6;i<9;i++) {
                TableVertex->set(i,bed_Radius/2.0*sin(2.0*M_PI*(i-6)/3.0),bed_Radius/2.0*cos(2.0*M_PI*(i-6)/3.0));
            }
            TableVertex->set(9,0,0);
        }
        else {
            TableVertex->set(6,0,0);
        }
    }
    bool fl = cbAutoManual->isChecked();
    TableVertex   ->setButtonVisible(!fl);
    TableVertex   ->setEditable(!fl);
}

// ---                           escher3d.com                           --- //
void CEscher3dWindow::calc()
{
    getParameters();
    convertIncomingEndstops();
    QString rslt;
    try {
        rslt = DoDeltaCalibration();
        result->setText(QString("<b>Success! %1<b>").arg(rslt));
        result->setStyleSheet("QLabel { background-color : green; }");
        convertOutgoingEndstops();
        setNewParameters();
        generateCommands();
        copyButton->setDisabled(false);
    }
    catch (...) {
        result->setText(QString("<b>Error! %1</b>").arg(rslt));
        result->setStyleSheet("QLabel { background-color : red; }");
        copyButton->setDisabled(true);
    }
}

// ---                           escher3d.com                           --- //
QString CEscher3dWindow::DoDeltaCalibration()
{
    if (__numFactors != 3 && __numFactors != 4 && __numFactors != 6 && __numFactors != 7) {
        return QString(tr("Error: %1 factors requested but only 3, 4, 6 and 7 supported")).arg(QString::number(__numFactors));
    }
    if (__numFactors > __numPoints) {
        return QString(tr("Error: need at least as many points as factors you want to calibrate"));
    }

    Debug.clear();

    // Transform the probing points to motor endpoints and store them in a matrix, so that we can do multiple iterations using the same data
    CMatrix probeMotorPositions(__numPoints, 3);
    double* corrections = new double[__numPoints];
    double initialSumOfSquares = 0.0;
    for (int i = 0; i < __numPoints; i++) {
        corrections[i] = 0.0;
        TVertex machinePos(BedProbePoints.at(i).X,BedProbePoints.at(i).Y,0.0);

        probeMotorPositions.setData(i,0,deltaParams.Transform(machinePos, 0));
        probeMotorPositions.setData(i,1,deltaParams.Transform(machinePos, 1));
        probeMotorPositions.setData(i,2,deltaParams.Transform(machinePos, 2));

        initialSumOfSquares += fsquare(BedProbePoints.at(i).Z);
    }

    Debug.append(probeMotorPositions.Print("Motor positions:"));

    // Do 1 or more Newton-Raphson iterations
    int    iteration = 0;
    double expectedRmsError;
    for (;;) {
        // Build a Nx7 matrix of derivatives with respect to xa, xb, yc, za, zb, zc, diagonal.
        CMatrix derivativeMatrix(__numPoints, __numFactors);
        for (int i = 0; i < __numPoints; i++) {
            for (int j = 0; j < __numFactors; j++) {
                derivativeMatrix.setData(i,j,deltaParams.ComputeDerivative(j,
                                                  probeMotorPositions.getData(i,0),
                                                  probeMotorPositions.getData(i,1),
                                                  probeMotorPositions.getData(i,2)));
            }
        }

        Debug.append(derivativeMatrix.Print("Derivative matrix:"));

        // Now build the normal equations for least squares fitting
        CMatrix normalMatrix(__numFactors, __numFactors + 1);
        for (int i = 0; i < __numFactors; i++) {
            for (int j = 0; j < __numFactors; j++) {
                double temp = derivativeMatrix.getData(0,i) * derivativeMatrix.getData(0,j);
                for (int k = 1; k < __numPoints; k++) {
                    temp += derivativeMatrix.getData(k,i) * derivativeMatrix.getData(k,j);
                }
                normalMatrix.setData(i,j,temp);
            }
            double temp = derivativeMatrix.getData(0,i) * (-(BedProbePoints.at(0).Z + corrections[0]));
            for (int k = 1; k < __numPoints; k++) {
                temp += derivativeMatrix.getData(k,i) * (-(BedProbePoints.at(k).Z + corrections[k]));
            }
            normalMatrix.setData(i,__numFactors,temp);
        }

        Debug.append(normalMatrix.Print("Normal matrix:"));

        QList<double> solution;
        normalMatrix.GaussJordan(solution,__numFactors);

        for (int i = 0; i < __numFactors; i++) {
            if (qIsNaN(solution[i])) {
                delete [] corrections;
                throw QString("Unable to calculate corrections. Please make sure the bed probe points are all distinct.");
            }
        }

        Debug.append(normalMatrix.Print("Solved matrix:"));

//        if (debug) {
//            DebugPrint(PrintVector("Solution", solution));

//            // Calculate and display the residuals
//            var residuals = [];
//            for (var i = 0; i < __numPoints; ++i) {
//                var r = zBedProbePoints[i];
//                for (var j = 0; j < numFactors; ++j) {
//                    r += solution[j] * derivativeMatrix.data[i][j];
//                }
//                residuals.push(r);
//            }
//            DebugPrint(PrintVector("Residuals", residuals));
//        }

        deltaParams.Adjust(__numFactors, solution, __normalise);

        // Calculate the expected probe heights using the new parameters
        {
            double* expectedResiduals = new double[__numPoints];
            double sumOfSquares = 0.0;
            for (int i = 0; i < __numPoints; i++) {
                for (int  axis = 0; axis < 3; axis++) {
                    double xxx= probeMotorPositions.getData(i,axis);
                    probeMotorPositions.setData(i,axis,xxx + solution[axis]);
                }
                double newZ = deltaParams.InverseTransform(probeMotorPositions.getData(i,0), probeMotorPositions.getData(i,1), probeMotorPositions.getData(i,2));
                corrections[i] = newZ;
                expectedResiduals[i] = BedProbePoints.at(i).Z + newZ;
                sumOfSquares += fsquare(expectedResiduals[i]);
            }

            expectedRmsError = sqrt(sumOfSquares/(double)(__numPoints));
//            DebugPrint(PrintVector("Expected probe error", expectedResiduals));
            delete [] expectedResiduals;
        }

        // Decide whether to do another iteration Two is slightly better than one, but three doesn't improve things.
        // Alternatively, we could stop when the expected RMS error is only slightly worse than the RMS of the residuals.
        iteration++;
        if (iteration == 2) { break; }
    }

    delete [] corrections;
    return QString("Calibrated %1 factors using %2 points, deviation before %3 after %4").
                   arg(QString::number(__numFactors),
                       QString::number(__numPoints),
                       QString::number(sqrt(initialSumOfSquares/(double)(__numPoints)),'f',2),
                       QString::number(expectedRmsError,'f',2));
//    return "Calibrated " + __numFactors + " factors using " + __numPoints + " points, deviation before " + Math.sqrt(initialSumOfSquares/(double)(__numPoints)).toFixed(2)
//            + " after " + expectedRmsError.toFixed(2);

}

// ---                           escher3d.com                           --- //
void CEscher3dWindow::getParameters()
{
    __normalise = normalise->isChecked();
    deltaParams = CDeltaParameters(oldrodlength  -> value(),
                                   oldradius     -> value(),
                                   oldhomedheight-> value(),
                                   oldxstop      -> value(),
                                   oldystop      -> value(),
                                   oldzstop      -> value(),
                                   oldxpos       -> value(),
                                   oldypos       -> value(),
                                   oldzpos       -> value());

    __bedRadius  = bedradius ->value();
    __numPoints  = numPoints ->value();
    __numFactors = numfactors->currentText().toInt();
    BedProbePoints.clear();
    for (int i=0;i<__numPoints;i++){
        BedProbePoints.append(TVertex( TableVertex->getX(i),
                                       TableVertex->getY(i),
                                      -TableVertex->getZ(i)));
    }
}

// ---                           escher3d.com                           --- //
void CEscher3dWindow::convertIncomingEndstops()
{
    double _endstopFactor = (double)(stepspermm->value());
    deltaParams.xstop /= _endstopFactor;
    deltaParams.ystop /= _endstopFactor;
    deltaParams.zstop /= _endstopFactor;

}

// ---                           escher3d.com                           --- //
void CEscher3dWindow::convertOutgoingEndstops()
{
    double _endstopFactor = stepspermm->value();
    deltaParams.xstop *= _endstopFactor;
    deltaParams.ystop *= _endstopFactor;
    deltaParams.zstop *= _endstopFactor;

}

// ---                           escher3d.com                           --- //
void CEscher3dWindow::setNewParameters()
{
    newxstop      ->setText(QString::number((int)(deltaParams.xstop+0.5)));
    newystop      ->setText(QString::number((int)(deltaParams.ystop+0.5)));
    newzstop      ->setText(QString::number((int)(deltaParams.zstop+0.5)));
    newrodlength  ->setText(QString::number(deltaParams.diagonal   ,'f',2));
    newradius     ->setText(QString::number(deltaParams.radius     ,'f',2));
    newhomedheight->setText(QString::number(deltaParams.homedHeight,'f',2));
    newxpos       ->setText(QString::number(deltaParams.xadj       ,'f',2));
    newypos       ->setText(QString::number(deltaParams.yadj       ,'f',2));
    newzpos       ->setText(QString::number(deltaParams.zadj       ,'f',2));

}

// ---                           escher3d.com                           --- //
void CEscher3dWindow::copyToInitial() {
    oldrodlength  ->setValue(deltaParams.diagonal);
    oldradius     ->setValue(deltaParams.radius);
    oldhomedheight->setValue(deltaParams.homedHeight);
    oldxstop      ->setValue((int)(deltaParams.xstop+0.5));
    oldystop      ->setValue((int)(deltaParams.ystop+0.5));
    oldzstop      ->setValue((int)(deltaParams.zstop+0.5));
    oldxpos       ->setValue(deltaParams.xadj);
    oldypos       ->setValue(deltaParams.yadj);
    oldzpos       ->setValue(deltaParams.zadj);
}


void CEscher3dWindow::generateCommands()
{

}

void CEscher3dWindow::on_oldradius_valueChanged(double arg1)
{
    bedradius->setMaximum(arg1);
}

void CEscher3dWindow::on_numPoints_valueChanged(int arg1)
{
    if (arg1<oldNumPoints){
        if      (arg1<7)  arg1 = 4;
        else if (arg1<10) arg1 = 7;
        else              arg1 = 10;
    }
    else{
        if      (arg1>7) arg1 = 10;
        else if (arg1>4) arg1 = 7;
        else             arg1 = 4;
    }
    numPoints->setValue(arg1);
    oldNumPoints = arg1;
    calcProbePoints();
}

void CEscher3dWindow::on_bedradius_valueChanged(double arg1)
{
    Q_UNUSED(arg1);
    calcProbePoints();
}

void CEscher3dWindow::on_numfactors_currentIndexChanged(const QString &arg1)
{
    if (arg1=="3")
        factorHelp->setText(tr("endstop corrections only"));
    else if (arg1=="4")
        factorHelp->setText(tr("endstop corrections and delta radius"));
    else if (arg1=="6")
        factorHelp->setText(tr("endstop corrections, delta radius, and two tower angular position corrections"));
    else if (arg1=="7")
        factorHelp->setText(tr("endstop corrections, delta radius, two tower angular position corrections, and diagonal rod length"));
}

void CEscher3dWindow::on_copyButton_clicked()
{
    copyToInitial  ();
    QStringList script;
    int i = Printer.EEPROM->indexByStartName("Tower X endstop offset");
    if (i>=0){
        Printer.EEPROM->setParameterValue(i,(qint32)(deltaParams.xstop+0.5));
        script.append(Printer.EEPROM->at(i)->ToCmdString());
    }
    i = Printer.EEPROM->indexByStartName("Tower Y endstop offset");
    if (i>=0){
        Printer.EEPROM->setParameterValue(i,(qint32)(deltaParams.ystop+0.5));
        script.append(Printer.EEPROM->at(i)->ToCmdString());
    }
    i = Printer.EEPROM->indexByStartName("Tower Z endstop offset");
    if (i>=0){
        Printer.EEPROM->setParameterValue(i,(qint32)(deltaParams.zstop+0.5));
        script.append(Printer.EEPROM->at(i)->ToCmdString());
    }
    i = Printer.EEPROM->indexByStartName("Diagonal rod length");
    if (i>=0){
        Printer.EEPROM->setParameterValue(i,(float)(deltaParams.diagonal));
        script.append(Printer.EEPROM->at(i)->ToCmdString());
    }
    i = Printer.EEPROM->indexByStartName("Horizontal rod radius at 0,0");
    if (i>=0){
        Printer.EEPROM->setParameterValue(i,(float)(deltaParams.radius));
        script.append(Printer.EEPROM->at(i)->ToCmdString());
    }
    i = Printer.EEPROM->indexByStartName("Z max length");
    if (i>=0){
        Printer.EEPROM->setParameterValue(i,(float)(deltaParams.homedHeight));
        script.append(Printer.EEPROM->at(i)->ToCmdString());
    }

    i = Printer.EEPROM->indexByStartName("Alpha A(210)");
    if (i>=0){
        Printer.EEPROM->setParameterValue(i,(float)(210.0 + deltaParams.xadj));
        script.append(Printer.EEPROM->at(i)->ToCmdString());
    }
    i = Printer.EEPROM->indexByStartName("Alpha B(330)");
    if (i>=0){
        Printer.EEPROM->setParameterValue(i,(float)(330.0 + deltaParams.yadj));
        script.append(Printer.EEPROM->at(i)->ToCmdString());
    }
    i = Printer.EEPROM->indexByStartName("Alpha C(90)");
    if (i>=0){
        Printer.EEPROM->setParameterValue(i,(float)(90.0 + deltaParams.zadj));
        script.append(Printer.EEPROM->at(i)->ToCmdString());
    }
    Printer.sendScript(script);
}

void CEscher3dWindow::updateControls()
{
    bool main= true;
    if (state==Active) main = false;
    stepspermm     ->setEnabled(main);
    oldxstop       ->setEnabled(main);
    oldystop       ->setEnabled(main);
    oldzstop       ->setEnabled(main);
    oldrodlength   ->setEnabled(main);
    oldradius      ->setEnabled(main);
    oldhomedheight ->setEnabled(main);
    oldxpos        ->setEnabled(main);
    oldypos        ->setEnabled(main);
    oldzpos        ->setEnabled(main);
    bedradius      ->setEnabled(main);
    numPoints      ->setEnabled(main);
    numfactors     ->setEnabled(main);
    dsbHeight      ->setEnabled(main);
    pbStart        ->setEnabled(main);
    normalise      ->setEnabled(main);
    pushButton     ->setEnabled(main);
    autoCheckBox   ->setEnabled(main);
    pbRecalc->setEnabled(TableVertex->isAllZValueReady());
}

