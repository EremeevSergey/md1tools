#include "2dchart.h"
#include <QFontMetrics>
#include <QBrush>
#include <QPen>

TIntFloatPoint::TIntFloatPoint()
{
    X=0;Y=0;
}

TIntFloatPoint::TIntFloatPoint(int x, float y)
{
    X=x; Y=y;
}

/*****************************************************************************\
*                                                                             *
\*****************************************************************************/
C2dChart::C2dChart(QWidget *parent) : QWidget(parent)
{
    XMax    = 10;
    YMax    = 1.0;
    XLeft   = 0;
    XRight  = 0;
    YTop    = 0;
    YBottom = 0;
    XStep   = 1;
    YStep   = 0.1;
}

C2dChart::~C2dChart()
{

}

QSize C2dChart::sizeHint() const
{
    return QSize(50,50);
}

void C2dChart::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);
    calculateGeometry();
}

void C2dChart::append  (const TIntFloatPoint& value)
{
    Points.append(value);
    qSort(Points);
    calculateGeometry();
    repaint();
}

void C2dChart::paintEvent (QPaintEvent * event)
{
    QWidget::paintEvent(event);
    QPainter painter;
    painter.begin(this);{
        updateUi(&painter);
    }
    painter.end();
}

#define DELTA 5
#define DIGIT_OFFSET 3
void C2dChart::calculateGeometry()
{
//    int  XLeft,XRight;
//    int  YTop,YBottom;
    QSize s = this->size();
    XRight = s.width() - DELTA;
    YTop   = DELTA;
    int ywidth=0;//,xwidth=0;
    int xheight;
    QFontMetrics fm = fontMetrics();
    xheight = fm.height();
    if (Points.isEmpty()){
//        xwidth=fm.boundingRect("10").width();
        ywidth=fm.boundingRect("1.0000").width();
        XMax    = 10;
        YMax    = 1.0;
        XStep   = 1;
        YStep   = 0.1;
    }
    else{
        XMax    = 0;
        YMax    = 0.0;
        for (int i=0;i<Points.size();i++){
//            int a = fm.boundingRect(QString::number(Points.at(i).X)).width();
//            if (xwidth<a) xwidth = a;
            if (XMax<Points.at(i).X) XMax=Points.at(i).X;
            if (YMax<Points.at(i).Y) YMax=Points.at(i).Y*1.1;
            int a = fm.boundingRect(QString::number(Points.at(i).Y,'f',4)).width();
            if (ywidth<a) ywidth = a;
        }
    }

    if (XMax<10) XMax = 10;
    float x = XMax/10.0;
    XStep = int(x+0.99);
    //if (YMax<1) YMax = 1;
    YStep = YMax/10.0;
    XLeft = DELTA + ywidth+DIGIT_OFFSET;
    YBottom = s.height() - DELTA - xheight - DIGIT_OFFSET;
}

void C2dChart::updateUi   (QPainter* painter)
{
    QPen pen(Qt::black);
    pen.setWidth(1);
    painter->setPen(pen);
    QBrush br(Qt::white);
    painter->setBrush(br);
    painter->drawRect(XLeft,YTop,XRight-XLeft,YBottom-YTop);
    pen.setColor(Qt::gray);
    painter->setPen(pen);
    QFontMetrics fm = fontMetrics();
    for (int i=1;i<10;i++){
        float d = XLeft+float(XRight-XLeft)*i/10.0;
        painter->drawLine(QPointF(d,YTop),QPointF(d,YBottom));
        d = YTop+float(YBottom-YTop)*i/10.0;
        painter->drawLine(QPointF(XLeft,d),QPointF(XRight,d));
    }
    pen.setColor(Qt::black);
    painter->setPen(pen);
    for (int i=1;i<10;i++){
        QString str = QString::number(0+XStep*i);
        QRectF r = fm.boundingRect(str);
        float w = r.width()/2.0;
        r.moveTopLeft(QPointF(XLeft+float(XRight-XLeft)/10.0*i-w,YBottom+DIGIT_OFFSET));
        painter->drawText(r,str);
    }
    for (int i=0;i<10;i++){
        QString str = QString::number(0.0+YStep*float(i),'f',4);
        QRectF r = fm.boundingRect(str);
        float w = r.height()/2.0;
        r.moveTopLeft(QPointF(XLeft - DIGIT_OFFSET - r.width(),YBottom - float(YBottom-YTop)/10.0*i-w));
        painter->drawText(r,str);
    }
    // Рисуем график
    pen.setColor(Qt::blue);
    pen.setWidth(2);
    painter->setPen(pen);
    if (Points.size()==1){
        painter->drawPoint(getPoint(0));
    }
    else if (Points.size()>1){
        for (int i=1;i<Points.size();i++){
        painter->drawLine(getPoint(i-1),getPoint(i));
        }
    }
}

QPointF C2dChart::getPoint(int i)
{
    float dx = float(XRight-XLeft)/10.0;
    float dy = float(YBottom-YTop)/10.0;
    float xx = Points.at(i).X;
    float yy = Points.at(i).Y;
    float x = XLeft + xx*dx/float(XStep);
    float y = YBottom - yy*dy/YStep;
    return QPointF(x,y);
}
