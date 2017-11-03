#ifndef BED_BASIC_WIDGET_H
#define BED_BASIC_WIDGET_H
#include <QWidget>
#include <QPainter>
#include <QResizeEvent>
//---------------------------------------------------------------------------//
//                       Базовый клас для "кроватей"                         //
//---------------------------------------------------------------------------//
class CBedWidgetBasic : public QWidget
{
    Q_OBJECT
public:
    void     setBedRadius(double);
    double   getBedRadius(){return planeRadius;}
    // Преобразование координат
    // 1. Полярные координаты
    QPointF  fromPolarToScreen (double rad,double grad);
    QPointF  fromPolarToPrinter(double rad,double grad);
    QPointF  fromScreenToPrinter(double x, double y);
    QPointF  fromPrinterToScreen(double x,double y);
public:
    explicit CBedWidgetBasic(QWidget *parent = 0);
    void          update();
    QSize         sizeHint() const;
protected:
    double        realToWidget(double);
    virtual void  draw(QPainter&){;}
    virtual void  updateUi(){;}
protected:
    void            resizeEvent(QResizeEvent * event);
    void            paintEvent (QPaintEvent * event);
    void            calculateGeometry();
    virtual QSizeF  __getRailSize() const;
    QRectF          getBedRect() const;
protected:
    // Параметры кравати
    double  planeRadius;
    QPointF planeCenter;
    // Башни
    QRect  RectA;
    QRect  RectB;
    QRect  RectC;
    QRect  RectAOpp;
    QRect  RectBOpp;
    QRect  RectCOpp;
    // Масштаб
    double scaleX,scaleY;
};

#endif // BED_BASIC_WIDGET_H
