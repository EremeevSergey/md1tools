#ifndef PLANE_WND_H
#define PLANE_WND_H

#include <QWidget>
#include <QPaintEvent>
#include <QList>
#include <QRectF>

#include "ui_plane_wnd.h"
#include "../core/core.h"
#include "../gui/basewnd.h"


/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
class CPlaneWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CPlaneWidget(QWidget *parent = 0);
    ~CPlaneWidget();
    void    setTestRadius(double rad);
    double  getTestRadius(){return Radius;}
    int     count(){return Vertices.count();}
    TVertex at(int i);
    void    setAt(int i,const TVertex& vertex);
    void    setMeshSize(int size);
    int     getMeshSize(){return meshSize;}
    void    setZScale(double size);
    double  getZScale(){return ZScale;}
    void    clear();
    void    append(const TVertex& vertex);
    void    clearVertices();
protected:
    double Radius;
    double ZScale;
    int meshSize;
    QList<TVertex> Vertices;
    double Xa,Ya;
    QRectF RectA;
    double Xb,Yb;
    QRectF RectB;
    double Xc,Yc;
    QRectF RectC;
    double planeCenterX,planeCenterY;
    double scaleX,scaleY;
    double planeRadius;
    double planeHwRadius;
protected:
    void   updateVertices   ();
    void   showPlane        ();
    void   showPrinter      (QPainter& painter);
    void   showVertexRect   (QPainter& painter,int index);
    void   calculateGeometry(double rect_size);
    QColor getRectColor     (double val);
    void   paintEvent       (QPaintEvent * event);
    bool   event            (QEvent * pe);
    QRectF getVertexRect    (const TVertex& ver);
private:
};

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
class CPlaneWindow : public CBaseWindow,public Ui::plane_wnd
{
    Q_OBJECT
public:
    explicit CPlaneWindow(QWidget *parent = 0);
    ~CPlaneWindow();
    void    setTestRadius(double rad){Plane->setTestRadius(rad);}
protected:
    CPlaneWidget* Plane;
    int  currentVertexIndex;
protected:
    bool gotoxyz       ();
    void displayStat   ();
    void updateControls();
    void mainLoop      ();
    void activeLoop    ();
private slots:
    void slotCommandExecuted();
    void on_dsbRadius_valueChanged(double arg1);
    void on_dsbHeight_valueChanged(double arg1);
    void on_sbMeshSize_valueChanged(int arg1);
    void on_ZScaleSlider_valueChanged(int value);
    void on_pbSave_clicked();
    void on_pbClear_clicked();
    void on_pbLoad_clicked();
    void on_pbStart_clicked();
    void on_pbStop_clicked();
private:
};

#endif // PLANE_WND_H
