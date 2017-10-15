#include "escher3d_wnd.h"
#include <math.h>
#include <QDebug>
#include "../common.h"

/*****************************************************************************\
*                               escher3d.com                                  *
\*****************************************************************************/
double fsquare(double x)
{
    return x*x;
}

double degreesToRadians = M_PI/180.0;

CDeltaParameters& CDeltaParameters::operator=(const CDeltaParameters& right) {
        //проверка на самоприсваивание
        if (this == &right) return *this;
        diagonal=right.diagonal;
        radius=right.radius;
        homedHeight=right.homedHeight;
        xstop=right.xstop;
        ystop=right.ystop;
        zstop=right.zstop;
        xadj=right.xadj;
        yadj=right.yadj;
        zadj=right.zadj;
        Recalc();
        return *this;
    }

CDeltaParameters::CDeltaParameters(double d,double r,double h,double xs,double ys,double zs,double xa,double ya,double za)
{
    diagonal   = d;
    radius     = r;
    homedHeight= h;
    xstop      = xs;
    ystop      = ys;
    zstop      = zs;
    xadj       = xa;
    yadj       = ya;
    zadj       = za;
    Recalc();
}

void CDeltaParameters::Recalc()
{
    towerX.clear();
    towerY.clear();
    towerX.append(-(radius * cos((30+xadj)*degreesToRadians)));
    towerY.append(-(radius * sin((30+xadj)*degreesToRadians)));
    towerX.append(+(radius * cos((30-yadj)*degreesToRadians)));
    towerY.append(-(radius * sin((30-yadj)*degreesToRadians)));
    towerX.append(-(radius * sin(zadj*degreesToRadians)));
    towerY.append(+(radius * cos(zadj*degreesToRadians)));

    Xbc = towerX.at(2) - towerX.at(1);
    Xca = towerX.at(0) - towerX.at(2);
    Xab = towerX.at(1) - towerX.at(0);
    Ybc = towerY.at(2) - towerY.at(1);
    Yca = towerY.at(0) - towerY.at(2);
    Yab = towerY.at(1) - towerY.at(0);
    coreFa = fsquare(towerX.at(0)) + fsquare(towerY.at(0));
    coreFb = fsquare(towerX.at(1)) + fsquare(towerY.at(1));
    coreFc = fsquare(towerX.at(2)) + fsquare(towerY.at(2));
    Q = 2 * (Xca * Yab - Xab * Yca);
    Q2 = fsquare(Q);
    D2 = fsquare(diagonal);

    // Calculate the base carriage height when the printer is homed.
    double tempHeight = diagonal;		// any sensible height will do here, probably even zero
    homedCarriageHeight = homedHeight + tempHeight - InverseTransform(tempHeight,tempHeight,tempHeight);
}

double CDeltaParameters::InverseTransform(double Ha,double Hb,double Hc)
{
    double Fa = coreFa + fsquare(Ha);
    double Fb = coreFb + fsquare(Hb);
    double Fc = coreFc + fsquare(Hc);

    // Setup PQRSU such that x = -(S - uz)/P, y = (P - Rz)/Q
    double P = (Xbc * Fa) + (Xca * Fb) + (Xab * Fc);
    double S = (Ybc * Fa) + (Yca * Fb) + (Yab * Fc);

    double R = 2 * ((Xbc * Ha) + (Xca * Hb) + (Xab * Hc));
    double U = 2 * ((Ybc * Ha) + (Yca * Hb) + (Yab * Hc));

    double R2 = fsquare(R), U2 = fsquare(U);

    double A = U2 + R2 + Q2;
    double minusHalfB = S * U + P * R + Ha * Q2 + towerX.at(0) * U * Q - towerY.at(0) * R * Q;
    double C = fsquare(S + towerX.at(0) * Q) + fsquare(P - towerY.at(0) * Q) + (fsquare(Ha) - D2) * Q2;

    double rslt = (minusHalfB - sqrt(fsquare(minusHalfB) - A * C)) / A;
    if (qIsNaN(rslt)) {
        throw QString("At least one probe point is not reachable. Please correct your delta radius, diagonal rod length, or probe coordniates.");
    }
    return rslt;

}

double CDeltaParameters::Transform(const TVertex& machinePos,int axis)
{
//    return machinePos[2] + Math.sqrt(this.D2 - fsquare(machinePos[0] - this.towerX[axis]) -
//                                               fsquare(machinePos[1] - this.towerY[axis]));
    return machinePos.Z + sqrt(D2 - fsquare(machinePos.X - towerX.at(axis)) -
                                    fsquare(machinePos.Y - towerY.at(axis)));

}

double CDeltaParameters::ComputeDerivative (int deriv,double ha,double hb,double hc)
{
    double perturb = 0.2;			// perturbation amount in mm or degrees
    CDeltaParameters hiParams (diagonal, radius, homedHeight, xstop, ystop, zstop, xadj, yadj, zadj);
    CDeltaParameters loParams (diagonal, radius, homedHeight, xstop, ystop, zstop, xadj, yadj, zadj);
    switch(deriv)
    {
    case 0:
    case 1:
    case 2:
        break;

    case 3:
        hiParams.radius += perturb;
        loParams.radius -= perturb;
        break;

    case 4:
        hiParams.xadj += perturb;
        loParams.xadj -= perturb;
        break;

    case 5:
        hiParams.yadj += perturb;
        loParams.yadj -= perturb;
        break;

    case 6:
        hiParams.diagonal += perturb;
        loParams.diagonal -= perturb;
        break;
    }

    hiParams.Recalc();
    loParams.Recalc();

    double zHi = hiParams.InverseTransform((deriv == 0) ? ha + perturb : ha, (deriv == 1) ? hb + perturb : hb, (deriv == 2) ? hc + perturb : hc);
    double zLo = loParams.InverseTransform((deriv == 0) ? ha - perturb : ha, (deriv == 1) ? hb - perturb : hb, (deriv == 2) ? hc - perturb : hc);

    return (zHi - zLo)/(2.0 * perturb);
}

void CDeltaParameters::Adjust (int numFactors, QList<double>& v, bool norm)
{
    double oldCarriageHeightA = homedCarriageHeight + xstop;	// save for later

    // Update endstop adjustments
    xstop += v.at(0);
    ystop += v.at(1);
    zstop += v.at(2);
    if (norm) {
        NormaliseEndstopAdjustments();
    }

    if (numFactors >= 4) {
        radius += v.at(3);

        if (numFactors >= 6) {
            xadj += v.at(4);
            yadj += v.at(5);

            if (numFactors == 7) {
                diagonal += v.at(6);
            }
        }
        Recalc();
    }

    // Adjusting the diagonal and the tower positions affects the homed carriage height.
    // We need to adjust homedHeight to allow for this, to get the change that was requested in the endstop corrections.
    double heightError = homedCarriageHeight + xstop - oldCarriageHeightA - v.at(0);
    homedHeight -= heightError;
    homedCarriageHeight -= heightError;
}

void CDeltaParameters::NormaliseEndstopAdjustments()
{
    double eav = qMin(xstop, qMin(ystop,zstop));
    xstop -= eav;
    ystop -= eav;
    zstop -= eav;
    homedHeight += eav;
    homedCarriageHeight += eav;				// no need for a full recalc, this is sufficient
}

/*----------------------------------------------------------------------------\
/                                                                             \
/----------------------------------------------------------------------------*/
CMatrix::CMatrix(int r,int c)
{
    Rows = r;
    Cols = c+1;
    data = new double [Rows*Cols];
    for(int i=0;i<Rows*Cols;i++)
        data[i] = 0.0;
}

CMatrix::~CMatrix()
{
    delete [] data;
}

void CMatrix::SwapRows (int i, int j, int numCols)
{
    if (i!=j){
        for (int k=0;k<numCols;k++) {
            double temp = getData(i,k);// [i][k];
            setData(i,k,getData(j,k));//this.data[i][k] = this.data[j][k];
            setData(j,k,temp);
        }
    }
}

QString CMatrix::Print(const QString& tag)
{
    QString rslt = tag + " {<br/>";
    for (int i = 0; i < Rows; ++i) {
//		var row = this.data[i];
        rslt += ' ';
        for (int j = 0; j < Cols; ++j) {
            rslt +=  QString::number(getData(i,j),'f',4);//  row[j].toFixed(4);
            if (j + 1 < Cols) rslt += ", ";
        }
        rslt += "<br/>";
    }
    rslt += '}';
    return rslt;
}

// Perform Gauus-Jordan elimination on a matrix with numRows rows and (njumRows + 1) columns
void CMatrix::GaussJordan(QList<double>& solution, int numRows)
{
    for (int i = 0; i < numRows; i++) {
        // Swap the rows around for stable Gauss-Jordan elimination
        double vmax = abs(getData(i,i));
        for (int j = i + 1; j < numRows; j++) {
            double rmax = abs(getData(j,i));
            if (rmax > vmax) {
                SwapRows(i, j, numRows + 1);
                vmax = rmax;
            }
        }

        // Use row i to eliminate the ith element from previous and subsequent rows
        double v = getData(i,i);
        for (int j = 0; j < i; j++) {
            double factor = getData(j,i)/v;
            setData(j,i,0.0);
            for (int k = i + 1; k <= numRows; k++) {// k <= numRows!!!!!!!!!!!!!!!!!!!!!!!!!
//                this.data[j][k] -= this.data[i][k] * factor;
                setData(j,k,getData(j,k) - getData(i,k) * factor);
            }
        }

        for (int j = i + 1; j < numRows; j++) {
            double factor = getData(j,i)/v;
            setData(j,i,0.0);
            for (int k = i + 1; k <= numRows; k++) {
//                this.data[j][k] -= this.data[i][k] * factor;
                setData(j,k,getData(j,k) - getData(i,k) * factor);
            }
        }
    }

    for (int i = 0; i < numRows; i++) {
        solution.append(getData(i,numRows)/ getData(i,i));
    }
}

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
QVertexTableModel::QVertexTableModel(QObject * parent):QAbstractTableModel(parent)
{
}

void QVertexTableModel::clear()
{
    beginResetModel();
    Points.clear();
    endResetModel();
}


void QVertexTableModel::append(const TVertex& ver)
{
    beginResetModel();
    Points.append(ver);
    endResetModel();
}

const TVertex& QVertexTableModel::at(int index)
{
    return Points.at(index);
}

void QVertexTableModel::setAt(int index,const TVertex& ver)
{
    if (index>=0 && index<Points.size()){
        beginResetModel();
        Points[index] = ver;
        endResetModel();
    }
}

void QVertexTableModel::setAt(int index,double x, double y, double z)
{
    if (index>=0 && index<Points.size()){
        beginResetModel();
        Points[index] = TVertex(x,y,z);
        endResetModel();
    }
}

int QVertexTableModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return Points.size();
}

int QVertexTableModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant QVertexTableModel::data(const QModelIndex & index, int role) const
{
    if (index.isValid()){
        int      col = index.column();
        int      row = index.row();
        if (row>=0 && row<Points.size()){
            if (role==Qt::DisplayRole){
            TVertex ver = Points.at(row);
            if (col==0 && !qIsNaN(ver.X)) return QString::number(ver.X,'f',2);
            if (col==1 && !qIsNaN(ver.Y)) return QString::number(ver.Y,'f',2);
            if (col==2 && !qIsNaN(ver.Z)) return QString::number(ver.Z,'f',2);
            }
        }
    }
    return QVariant();
}

QVariant QVertexTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole){
        if (section==0) return "X";
        if (section==1) return "Y";
        if (section==2) return "Z";
    }
    return QAbstractTableModel::headerData(section,orientation,role);
}

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
CEscher3dWindow::CEscher3dWindow(QWidget *parent) :
    CBaseWindow(parent),Ui::escher3d_wnd()
{
    setupUi(this);
    Model = new QVertexTableModel();
    tvPoints->setModel(Model);
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
    dsbProbeHeight->setValue(5.18);
    step  = NoStep;
    state = Stoped;
    currectVertexIndex = 0;
    connect (&Printer,SIGNAL(signalCommandExecuted()),this,SLOT(slotCommandExecuted()));
    pbRecalc->setEnabled(false);
    updateControls();
    calcProbePoints();
}

CEscher3dWindow::~CEscher3dWindow()
{
}

// ---                           escher3d.com                           --- //
void CEscher3dWindow::calcProbePoints()
{
    Model->clear();
/*    Model->append(TVertex( 0    , 75   ,-0.96));
    Model->append(TVertex( 64.95, 37.50,-0.72));
    Model->append(TVertex( 64.95,-37.50,-0.76));
    Model->append(TVertex( 0    ,-75.00,-0.56));
    Model->append(TVertex(-64.95,-37.50,-0.46));
    Model->append(TVertex(-64.95, 37.50,-0.45));
    Model->append(TVertex( 0    , 37.50,-0.29));
    Model->append(TVertex( 32.48,-18.75,-0.13));
    Model->append(TVertex(-32.48,-18.75,-0.08));
    Model->append(TVertex( 0    , 0    ,-0.02));
*/
    int    num_Points = numPoints->value();
    double bed_Radius = bedradius->value();
    Model->clear();
    if (num_Points==4) {
        for (int i=0;i<3;i++) {
            Model->append(TVertex(bed_Radius*sin((2.0*M_PI*i)/3.0),
                                  bed_Radius*cos((2.0*M_PI*i)/3.0)));
        }
        Model->append(TVertex(0,0));
    }
    else {
        if (num_Points>=7) {
            for (int i=0;i<6;i++) {
                Model->append(TVertex(bed_Radius*sin((2.0*M_PI*i)/6.0),
                                      bed_Radius*cos((2.0*M_PI*i)/6.0)));
            }
        }
        if (num_Points>=10) {
            for (int i=6;i<9;i++) {
                Model->append(TVertex(bed_Radius/2.0*sin((2.0*M_PI*(i-6))/3.0),
                                      bed_Radius/2.0*cos((2.0*M_PI*(i-6))/3.0)));
            }
            Model->append(TVertex(0,0));
        }
        else {
            Model->append(TVertex(0,0));
        }
    }
}

// ---                           escher3d.com                           --- //
void CEscher3dWindow::calc()
{
    getParameters();
    convertIncomingEndstops();
    QString rslt;
    try {
        rslt = DoDeltaCalibration();
        result->setText(QString("&nbsp;Success! %1&nbsp;").arg(rslt));
        result->setStyleSheet("QLabel { background-color : green; }");
//        document.getElementById("result").style.backgroundColor = "LightGreen";
        convertOutgoingEndstops();
        setNewParameters();
        generateCommands();
        copyButton->setDisabled(false);
    }
    catch (...) {
        result->setText(QString("&nbsp;Error! %1&nbsp;").arg(rslt));
        result->setStyleSheet("QLabel { background-color : red; }");
//        document.getElementById("result").style.backgroundColor = "LightPink";
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
            double temp = derivativeMatrix.getData(0,i) * -(BedProbePoints.at(0).Z + corrections[0]);
            for (int k = 1; k < __numPoints; k++) {
                temp += derivativeMatrix.getData(k,i) * -(BedProbePoints.at(k).Z + corrections[k]);
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
    deltaParams = CDeltaParameters(oldrodlength->value(),
                                     oldradius->value(),
                                     oldhomedheight->value(),
                                     oldxstop->value(),
                                     oldystop->value(),
                                     oldzstop->value(),
                                     oldxpos ->value(),
                                     oldypos ->value(),
                                     oldzpos ->value());

    __bedRadius  = bedradius ->value();
    __numPoints  = numPoints ->value();
    __numFactors = numfactors->currentText().toInt();
    BedProbePoints.clear();
    for (int i=0;i<__numPoints;i++){
        TVertex ver = Model->at(i);
        ver.Z=-ver.Z;
        BedProbePoints.append(ver);
    }

//    xBedProbePoints = [];
//    yBedProbePoints = [];
//    zBedProbePoints = [];
//    for (var i = 0; i < numPoints; ++i) {
//        xBedProbePoints.push(+document.getElementById("probeX" + i).value);
//        yBedProbePoints.push(+document.getElementById("probeY" + i).value);
//        zBedProbePoints.push(-document.getElementById("probeZ" + i).value);
//    }
}

// ---                           escher3d.com                           --- //
void CEscher3dWindow::convertIncomingEndstops()
{
    double _endstopFactor = 1.0/(double)(stepspermm->value());
    deltaParams.xstop *= _endstopFactor;
    deltaParams.ystop *= _endstopFactor;
    deltaParams.zstop *= _endstopFactor;

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
/*    newrodlength  ->setText(QString::number(deltaParams.diagonal   ,'f',2));
    newradius     ->setText(QString::number(deltaParams.radius     ,'f',2));
    newhomedheight->setText(QString::number(deltaParams.homedHeight,'f',2));
    newxstop      ->setText(QString::number(deltaParams.xstop      ,'f',0));
    newystop      ->setText(QString::number(deltaParams.ystop      ,'f',0));
    newzstop      ->setText(QString::number(deltaParams.zstop      ,'f',0));
    newxpos       ->setText(QString::number(deltaParams.xadj       ,'f',2));
    newypos       ->setText(QString::number(deltaParams.yadj       ,'f',2));
    newzpos       ->setText(QString::number(deltaParams.zadj       ,'f',2));
*/
//    oldrodlength  ->setValue(newrodlength  ->text().toDouble());
//    oldradius     ->setValue(newradius     ->text().toDouble());
//    oldhomedheight->setValue(newhomedheight->text().toDouble());
//    oldxstop      ->setValue(newxstop->text().toInt());
//    oldystop      ->setValue(newystop->text().toInt());
//    oldzstop      ->setValue(newzstop->text().toInt());
//    oldxpos       ->setValue(newxpos->text().toDouble());
//    oldypos       ->setValue(newypos->text().toDouble());
//    oldzpos       ->setValue(newzpos->text().toDouble());
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
/*    oldrodlength  ->setValue(deltaParams.diagonal);
    oldradius     ->setValue(deltaParams.radius);
    oldhomedheight->setValue(deltaParams.homedHeight);
    oldxstop      ->setValue((int)(deltaParams.xstop+0.5));
    oldystop      ->setValue((int)(deltaParams.ystop+0.5));
    oldzstop      ->setValue((int)(deltaParams.zstop+0.5));
    oldxpos       ->setValue(deltaParams.xadj);
    oldypos       ->setValue(deltaParams.yadj);
    oldzpos       ->setValue(deltaParams.zadj);*/
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
        Printer.EEPROM->setParameterValue(i,(float)(330.0 + deltaParams.zadj));
        script.append(Printer.EEPROM->at(i)->ToCmdString());
    }
    Printer.playScript(script);
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
    dsbProbeHeight ->setEnabled(main);
    pbStart        ->setEnabled(main);
    normalise      ->setEnabled(main);
}

void CEscher3dWindow::slotCommandExecuted()
{
    mainLoop();
}

void CEscher3dWindow::on_pbStart_clicked()
{
    copyButton->setDisabled(true);
    if (state==Stoped){
        start();
        calcProbePoints();
        pbRecalc->setEnabled(false);
        state= Active;
        step = GoHome;
        currectVertexIndex = 0;
//        Printer.Connection->writeLine("M321");
//        Printer.Connection->writeLine("M322");
        Printer.cmdGoHomeAll();
        updateControls();
    }
}

void CEscher3dWindow::mainLoop()
{
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
    TVertex ver;
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
        Printer.cmdGetZProbeValue();
        step = ZProbe;
        break;
    case ZProbe:
        ver=Model->at(currectVertexIndex);
        z = Printer.ZProbe.Z;
        if (!qIsNaN(z)) ver.Z = z + dsbProbeHeight->value() -dsbHeight->value();
        else            ver.Z = z;
        Model->setAt(currectVertexIndex,ver);
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
    int count = Model->size();
    if (count>currectVertexIndex){
        TVertex ver = Model->at(currectVertexIndex);
        ver.Z = dsbHeight->value()+dsbProbeHeight->value();
        step = GoToXYZ;
        Printer.cmdGoToXYZ(ver.X,ver.Y,ver.Z);
        return true;
    }
    return false;
}


void CEscher3dWindow::on_pbRecalc_clicked()
{
    calc();
    updateControls();
}
