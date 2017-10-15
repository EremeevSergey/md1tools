#ifndef ESCHER3D_H
#define ESCHER3D_H
#include <QList>
#include <QStringList>
#include "../../core/core.h"


/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
class CDeltaParameters
{
public:
    double diagonal;
    double radius;
    double homedHeight;
    double xstop;
    double ystop;
    double zstop;
    double xadj;
    double yadj;
    double zadj;

    QList<double> towerX;
    QList<double> towerY;
    double Xbc;
    double Xca;
    double Xab;
    double Ybc;
    double Yca;
    double Yab;
    double coreFa;
    double coreFb;
    double coreFc;
    double Q;
    double Q2;
    double D2;
    double homedCarriageHeight;


    CDeltaParameters         (){;}
    CDeltaParameters         (double d,double r,double h,
                              double xs,double ys,double zs,
                              double xa,double ya,double za);
    void   Recalc            ();
    double InverseTransform  (double Ha, double Hb, double Hc);
    double Transform         (const TVertex& machinePos,int axis);
    double ComputeDerivative (int deriv,double ha,double hb,double hc);
    void   Adjust            (int numFactors, QList<double> &v, bool norm);
    void   NormaliseEndstopAdjustments();
    CDeltaParameters& operator=(const CDeltaParameters& right);
};

/*----------------------------------------------------------------------------\
/                                                                            \
/----------------------------------------------------------------------------*/
class CMatrix
{
public:
    CMatrix(int r,int c);
    ~CMatrix();
    double* data;
    void SwapRows (int i, int j, int numCols);
    inline void setData(int r,int c,double d){data[r*Cols + c] = d;}
    inline double getData(int r,int c){return data[r*Cols + c];}
    void   GaussJordan(QList<double>& solution, int numRows);
    QString Print(const QString& tag);
protected:
    int Rows;
    int Cols;
};

#endif // ESCHER3D_H
