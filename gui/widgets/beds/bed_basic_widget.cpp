#include "bed_widget.h"
#include <math.h>
#include <QSizeF>
#include <QRectF>
#include <QFontMetrics>

//---------------------------------------------------------------------------//
//                       Базовый клас для "кроватей"                         //
//---------------------------------------------------------------------------//
CBedWidgetBasic::CBedWidgetBasic(QWidget *parent):QWidget(parent)
{
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    planeRadius = 90.0;
}

QSizeF  CBedWidgetBasic::__getRailSize()  const
{
    return QSizeF(40,40);
}

void CBedWidgetBasic::paintEvent (QPaintEvent * event)
{
    QWidget::paintEvent(event);
    QPainter painter;
    painter.begin(this);
    draw(painter);
    painter.end();
}

void CBedWidgetBasic::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);
    update();
}

void CBedWidgetBasic::update()
{
    calculateGeometry();
    updateUi();
}

QSize CBedWidgetBasic::sizeHint() const
{
    QSizeF s = __getRailSize();
    return QSize(s.width()*4,s.height()*4);
}

#define CIRCLE_OFFSET 4
static float cos60 = cos(M_PI/3);
void CBedWidgetBasic::calculateGeometry()
{
    float w = width();
    float h = height();
    QSizeF s = __getRailSize();
    planeCenter = QPointF(w/2,h/2);
    float b = (height()-s.height()*2.0)/2.0;
    float x = planeCenter.x()-s.width();
    float y = b*cos60;
    RectA = QRect(planeCenter.x()-s.width()/2,0                ,s.width(),s.height());
    RectB = QRect(0                          ,planeCenter.y()+y,s.width(),s.height());
    RectC = QRect(w - s.width()              ,planeCenter.y()+y,s.width(),s.height());

    RectAOpp = QRect(planeCenter.x()-s.width()/2,h-s.height(),s.width(),s.height());
    RectBOpp = QRect(w-s.width(),planeCenter.y()-y-s.height(),s.width(),s.height());
    RectCOpp = QRect(0          ,planeCenter.y()-y-s.height(),s.width(),s.height());

    float a = sqrt(b*b*x*x/(b*b-y*y));

    scaleX=(a-CIRCLE_OFFSET)/planeRadius;
    scaleY=(b-CIRCLE_OFFSET)/planeRadius;
}

QRectF  CBedWidgetBasic::getBedRect() const
{
    return  QRectF(planeCenter.x()-planeRadius*scaleX,
                   planeCenter.y()-planeRadius*scaleY,
                   planeRadius*2*scaleX,
                   planeRadius*2*scaleY);
}

void CBedWidgetBasic::setBedRadius(double val)
{
    if (val!=planeRadius){
        planeRadius = val;
        update();
        repaint();
    }
}

//        -----    Преобразование координат   -----     //
QPointF CBedWidgetBasic::fromPolarToScreen(double rad, double grad)
{
    double x = rad*cos(grad);
    double y = rad*sin(grad);
    return fromPrinterToScreen(x,y);
}

QPointF CBedWidgetBasic::fromPolarToPrinter(double rad,double grad)
{
    double x = rad*cos(grad);
    double y = rad*sin(grad);
    return QPointF(x,y);
}

QPointF CBedWidgetBasic::fromScreenToPrinter(double x,double y)
{
    double sx = (x-planeCenter.x())/scaleX;
    double sy = (y-planeCenter.y())/scaleY;
    double a = -120.0/180.0*M_PI;
    return QPointF (sx*cos(a)-sy*sin(a),
                    sx*sin(a)+sy*cos(a));
}

QPointF CBedWidgetBasic::fromPrinterToScreen(double x, double y)
{
    double a = 120.0/180.0*M_PI;
    return QPointF (planeCenter.x()+(x*cos(a)-y*sin(a))*scaleX,
                    planeCenter.y()-(x*sin(a)+y*cos(a))*scaleY);
}



