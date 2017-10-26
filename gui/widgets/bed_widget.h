#ifndef BED_WIDGET_H
#define BED_WIDGET_H
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
    explicit CBedWidgetBasic(QWidget *parent = 0);
protected:
    double Radius;
    double Xa,Ya;
    QRectF RectA;
    double Xb,Yb;
    QRectF RectB;
    double Xc,Yc;
    QRectF RectC;
    double planeCenterX,planeCenterY;
    double scaleX,scaleY;
    double planeRadius;

    void resizeEvent(QResizeEvent * event);
    void paintEvent (QPaintEvent * event);

    virtual void  Draw();
    inline QRectF ScaleRect(const QRectF& rect){
        return QRectF(rect.x()    *scaleX,rect.y()     *scaleY,
                      rect.width()*scaleX,rect.height()*scaleY);
    }
private:
    void         ___DrawText (QPainter& painter,const QRectF& rect,const QString& name);
    void         calculateGeometry();

signals:

public slots:
};

#endif // BED_WIDGET_H
