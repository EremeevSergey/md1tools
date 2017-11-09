#include "black_bed_widget.h"
#include <math.h>
#include <QSizeF>
#include <QRectF>
#include <QFontMetrics>
#include <QtNumeric>

//---------------------------------------------------------------------------//
//                       Базовый клас для "кроватей"                         //
//---------------------------------------------------------------------------//
CBedWidgetBasic::CBedWidgetBasic(QWidget *parent):QWidget(parent)
{
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    planeRadius = 90.0;
}

CBedWidgetBasic::~CBedWidgetBasic()
{
    clearDecorators();
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
    for (int i=0,n=Decorators.size();i<n;i++){
        CBedDecoratorBase* d = Decorators.at(i);
        if (d && d->Visible){
            painter.save();
            d->draw(painter);
            painter.restore();
        }
    }
    painter.end();
}

void CBedWidgetBasic::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);
    update();
}

bool CBedWidgetBasic::event (QEvent * pe)
{
    for (int i=0,n=Decorators.size();i<n;i++){
        CBedDecoratorBase* d = Decorators.at(i);
        if (d && d->Visible)
            d->event(pe);
    }
    return QWidget::event(pe);
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

    scaleXpriv=(a-CIRCLE_OFFSET)/planeRadius;
    scaleYpriv=(b-CIRCLE_OFFSET)/planeRadius;
}

QRectF  CBedWidgetBasic::getBedRect(double proc) const
{
    return  QRectF(planeCenter.x()-planeRadius*proc*scaleXpriv,
                   planeCenter.y()-planeRadius*proc*scaleYpriv,
                   planeRadius*proc*2*scaleXpriv,
                   planeRadius*proc*2*scaleYpriv);
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
    if (!qIsNaN(rad) && !qIsNaN(grad)){
        double x = rad*cos(grad);
        double y = rad*sin(grad);
        return fromPrinterToScreen(x,y);
    }
    return QPointF(qQNaN(),qQNaN());
}

QPointF CBedWidgetBasic::fromPolarToPrinter(double rad,double grad)
{
    if (!qIsNaN(rad) && !qIsNaN(grad)){
        double x = rad*cos(grad);
        double y = rad*sin(grad);
        return QPointF(x,y);
    }
    return QPointF(qQNaN(),qQNaN());
}

QPointF CBedWidgetBasic::fromPrinterToPolar(double x,double y)
{
    if (!qIsNaN(x) && !qIsNaN(y)){
        double r = sqrt(x*x+y*y);
        double a;
        if      (x>0 && y>=0) a=atan(y/x);
        else if (x>0 && y< 0) a=atan(y/x)+2*M_PI;
        else if (x<0)         a=atan(y/x)+M_PI;
        else if (x==0 && y>0) a=M_PI/2.0;
        else if (x==0 && y<0) a=3*M_PI/2.0;
        else a=0;
        return QPointF(r,a);
    }
    return QPointF(qQNaN(),qQNaN());
}


QPointF CBedWidgetBasic::fromScreenToPrinter(double x,double y)
{
    if (!qIsNaN(x) && !qIsNaN(x)){
        double sx = (x-planeCenter.x())/scaleXpriv;
        double sy = -(y-planeCenter.y())/scaleYpriv;
        double a = 120.0/180.0*M_PI;
        return QPointF (sx*cos(a)-sy*sin(a),
                        sx*sin(a)+sy*cos(a));
    }
    return QPointF(qQNaN(),qQNaN());
}

QPointF CBedWidgetBasic::fromPrinterToScreen(double x, double y)
{
    if (!qIsNaN(x) && !qIsNaN(x)){
        double a = -120.0/180.0*M_PI;
        return QPointF (planeCenter.x()+(x*cos(a)-y*sin(a))*scaleXpriv,
                        planeCenter.y()-(x*sin(a)+y*cos(a))*scaleYpriv);
    }
    return QPointF(qQNaN(),qQNaN());
}

QPointF CBedWidgetBasic::fromPrinterToHuman(double x,double y)
{
    if (!qIsNaN(x) && !qIsNaN(x)){
        double a = -120.0/180.0*M_PI;
        return QPointF ((x*cos(a)-y*sin(a)),
                        (x*sin(a)+y*cos(a)));
    }
    return QPointF(qQNaN(),qQNaN());
}

QPointF CBedWidgetBasic::fromHumanToPrinter(double x,double y)
{
    if (!qIsNaN(x) && !qIsNaN(x)){
        double a = 120.0/180.0*M_PI;
        return QPointF ((x*cos(a)-y*sin(a)),
                        (x*sin(a)+y*cos(a)));
    }
    return QPointF(qQNaN(),qQNaN());
}

// Украшалки
void  CBedWidgetBasic::clearDecorators()
{
    for (int i=0,n=Decorators.size();i<n;i++){
        CBedDecoratorBase* d = Decorators.at(i);
        d->BedWidget=0;
        delete d;
    }
    Decorators.clear();
}

bool CBedWidgetBasic::addDecorator(CBedDecoratorBase* dec)
{
    int i = Decorators.indexOf(dec);
    if (i<0) {
        Decorators.append(dec);
        dec->BedWidget = this;
        return true;
    }
    return false;
}

void CBedWidgetBasic::removeDecorator(CBedDecoratorBase*dec)
{
    int i = Decorators.indexOf(dec);
    if (i>=0){
        dec->BedWidget=0;
        delete dec;
        Decorators.removeAt(i);
    }
}

void CBedWidgetBasic::moveDecoratorOnTop(CBedDecoratorBase* dec)
{
    int i = Decorators.indexOf(dec);
    if (i>=0){
        Decorators.removeAt(i);
        Decorators.append(dec);
    }
}
//---------------------------------------------------------------------------//
//                                Украшалки                                  //
//---------------------------------------------------------------------------//
CBedDecoratorBase::CBedDecoratorBase(CBedWidgetBasic* parent)
{
    Visible = true;
    Locked  = false;
    parent->addDecorator(this);
    BedWidget->repaint();
}

CBedDecoratorBase::~CBedDecoratorBase()
{
    if (BedWidget)
        BedWidget->removeDecorator(this);
}

void CBedDecoratorBase::update()
{
    if (!Locked)
        BedWidget->repaint();
}

void CBedDecoratorBase::setVisible(bool fl)
{
    Visible = fl;
    BedWidget->repaint();
}

void CBedDecoratorBase::unlock()
{
    Locked  = false;
    BedWidget->repaint();
}

void CBedDecoratorBase::moveOnTop()
{
    BedWidget->moveDecoratorOnTop(this);
}
