#ifndef ESCHER3D_WND_H
#define ESCHER3D_WND_H
#include <QAbstractTableModel>
#include <QList>
#include <QStringList>
#include <QModelIndex>
#include "../core/core.h"

#include "ui_escher3d_wnd.h"
#include "../gui/basewnd.h"

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


    CDeltaParameters(){;}
    CDeltaParameters(double d,double r,double h,double xs,double ys,double zs,double xa,double ya,double za);
    void   Recalc();
    double InverseTransform(double Ha, double Hb, double Hc);
    double Transform(const TVertex& machinePos,int axis);
    double ComputeDerivative (int deriv,double ha,double hb,double hc);
    void   Adjust (int numFactors, QList<double> &v, bool norm);
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

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
class QVertexTableModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    QVertexTableModel(QObject * parent = 0);
    void              clear();
    void              append(const TVertex& ver);
    int               size(){return Points.count();}
    const TVertex&    at(int index);
    void              setAt(int index,const TVertex& ver);
    void              setAt(int index,double x, double y, double z);

    int               rowCount(const QModelIndex & parent = QModelIndex()) const;
    int               columnCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant          data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant          headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
protected:
    QList<TVertex>    Points;
};

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
class CEscher3dWindow : public CBaseWindow, public Ui::escher3d_wnd
{
    Q_OBJECT

protected:
    void    calc();
    void    calcProbePoints();
    void    copyToInitial  ();
    void    getParameters();
    void    convertIncomingEndstops();
    void    convertOutgoingEndstops();
    void    setNewParameters();
    void    generateCommands();
    QString DoDeltaCalibration();
protected:
    bool             __normalise;
    CDeltaParameters deltaParams;
    double           __bedRadius;
    int              __numPoints;
    int              __numFactors;
    QList<TVertex>   BedProbePoints;
    QStringList      Debug;
protected:
    int  step;
    int  state;
    int  currectVertexIndex;
    bool gotoxyz       ();
    void updateControls();
    void mainLoop      ();
    void activeLoop    ();

private slots:
    void slotCommandExecuted();

public:
    explicit CEscher3dWindow(QWidget *parent = 0);
    ~CEscher3dWindow();
protected:
    QVertexTableModel* Model;
    int                oldNumPoints;
private slots:
    void on_oldradius_valueChanged(double arg1);

    void on_numPoints_valueChanged(int arg1);

    void on_bedradius_valueChanged(double arg1);

    void on_numfactors_currentIndexChanged(const QString &arg1);

    void on_pbStart_clicked();

    void on_copyButton_clicked();

    void on_pbRecalc_clicked();

private:
};

#endif // ESCHER3D_WND_H
