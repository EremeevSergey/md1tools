#include "escher3d.h"
#include <math.h>
#include <QDebug>
#include "../../common.h"

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
        diagonal    = right.diagonal;
        radius      = right.radius;
        homedHeight = right.homedHeight;
        xstop       = right.xstop;
        ystop       = right.ystop;
        zstop       = right.zstop;
        xadj        = right.xadj;
        yadj        = right.yadj;
        zadj        = right.zadj;
        Recalc();
        return *this;
    }

CDeltaParameters::CDeltaParameters(double d ,double  r,double  h,
                                   double xs,double ys,double zs,
                                   double xa,double ya,double za)
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
    double tempHeight = diagonal; // any sensible height will do here, probably even zero
    homedCarriageHeight = homedHeight + tempHeight -
            InverseTransform(tempHeight,tempHeight,tempHeight);
}

double CDeltaParameters::InverseTransform(double Ha,double Hb,double Hc)
{
    double Fa = coreFa + fsquare(Ha);
    double Fb = coreFb + fsquare(Hb);
    double Fc = coreFc + fsquare(Hc);

    double P = (Xbc * Fa) + (Xca * Fb) + (Xab * Fc);
    double S = (Ybc * Fa) + (Yca * Fb) + (Yab * Fc);

    double R = 2 * ((Xbc * Ha) + (Xca * Hb) + (Xab * Hc));
    double U = 2 * ((Ybc * Ha) + (Yca * Hb) + (Yab * Hc));

    double R2 = fsquare(R), U2 = fsquare(U);

    double A = U2 + R2 + Q2;
    double minusHalfB = S*U + P*R + Ha*Q2 +
                        towerX.at(0)*U*Q -
                        towerY.at(0)*R*Q;
    double C = fsquare(S+towerX.at(0)*Q) +
               fsquare(P-towerY.at(0)*Q) +
               (fsquare(Ha)-D2)*Q2;

    double rslt = (minusHalfB - sqrt(fsquare(minusHalfB) - A * C)) / A;
    if (qIsNaN(rslt)) {
        throw QString("At least one probe point is not reachable. Please correct your delta radius, diagonal rod length, or probe coordniates.");
    }
    return rslt;

}

double CDeltaParameters::Transform(const TVertex& machinePos,int axis)
{
    return machinePos.Z + sqrt(D2 - fsquare(machinePos.X - towerX.at(axis)) -
                                    fsquare(machinePos.Y - towerY.at(axis)));
}

double CDeltaParameters::ComputeDerivative (int deriv,
                                            double ha,double hb,double hc)
{
    double perturb = 0.2;			// perturbation amount in mm or degrees
    CDeltaParameters hiParams (diagonal, radius, homedHeight,
                               xstop, ystop, zstop,
                               xadj, yadj, zadj);
    CDeltaParameters loParams (diagonal, radius, homedHeight,
                               xstop, ystop, zstop,
                               xadj, yadj, zadj);
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

    double zHi = hiParams.InverseTransform((deriv == 0) ? ha + perturb :
                                                          ha, (deriv == 1) ? hb + perturb :
                                                                             hb, (deriv == 2) ? hc + perturb :
                                                                                                hc);
    double zLo = loParams.InverseTransform((deriv == 0) ? ha - perturb :
                                                          ha, (deriv == 1) ? hb - perturb :
                                                                             hb, (deriv == 2) ? hc - perturb :
                                                                                                hc);

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

    for (int i = 0; i < numRows; i++)
        solution.append(getData(i,numRows)/ getData(i,i));
}

