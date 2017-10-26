#include "bed_widget.h"
#include <math.h>
#include <QSizeF>
#include <QRectF>

#define RAIL_SIZE 0.05
//---------------------------------------------------------------------------//
//                       Базовый клас для "кроватей"                         //
//---------------------------------------------------------------------------//
CBedWidgetBasic::CBedWidgetBasic(QWidget *parent) : QWidget(parent)
{

}

void CBedWidgetBasic::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);
    calculateGeometry();
}

void CBedWidgetBasic::paintEvent (QPaintEvent * event)
{
    QWidget::paintEvent(event);
    Draw();
}

void CBedWidgetBasic::Draw()
{
    QPainter painter;
    painter.begin(this);{
        painter.setRenderHint(QPainter::Antialiasing,true);
        Qt::GlobalColor col = Qt::black;
        if (!isEnabled()) col = Qt::darkGray;
        painter.setPen(col);
        painter.setBrush(QBrush(col));
        // Рисуем элипс кровати
        painter.drawEllipse((planeCenterX - planeRadius)*scaleX,
                            (planeCenterY - planeRadius)*scaleY,
                            planeRadius*2.0*scaleX,
                            planeRadius*2.0*scaleY);
        // Прямоугольники рельс
        painter.drawRect(ScaleRect(RectA));
        painter.drawRect(ScaleRect(RectB));
        painter.drawRect(ScaleRect(RectC));
        // Названия рельс
        ___DrawText (painter,RectA,"X");
        ___DrawText (painter,RectC,"Y");
        ___DrawText (painter,RectB,"Z");
    }
    painter.end();
}

void CBedWidgetBasic::___DrawText (QPainter& painter,const QRectF& rect,const QString& name)
{
    painter.setPen(Qt::white);
    painter.drawText(ScaleRect(rect),name,Qt::AlignHCenter | Qt::AlignVCenter);
}


void CBedWidgetBasic::calculateGeometry()
{
    QSizeF ts = size();
    double rect_size;
    if (ts.width()>ts.height()) {
        scaleX=1.0;
        rect_size=ts.width();
        scaleY=ts.height()/rect_size;
    }
    else{
        scaleY=1.0;
        rect_size=ts.height();
        scaleX=ts.width()/rect_size;
    }

    double rail = rect_size*RAIL_SIZE;
    Xa = rect_size/2.0;
    Ya = rail;
    RectA = QRectF(Xa-rail/2.0,0,rail,rail);
    double radius = (rect_size - Ya)/2.0;
    planeRadius = radius-4.0;
    planeCenterX = Xa;
    planeCenterY = Ya+radius;
    double n = radius*cos(M_PI/6.0)*2.0;//cos(30)
    Yb = Ya + n*cos(M_PI/6.0);//cos(30)
    double m = n*sin(M_PI/6.0);
    Xb = Xa - m;
    RectB = QRectF(Xb-rail,Yb,rail,rail);
    Yc = Yb;
    Xc = Xa + m;
    RectC = QRectF(Xc,Yc,rail,rail);
}

