#ifndef BED_BASIC_WIDGET_H
#define BED_BASIC_WIDGET_H
#include <QWidget>
#include <QPainter>
#include <QResizeEvent>
#include <QList>

namespace CBed
{
    enum ECoordinateSystem{
        EScreen = 0,
        EPrinter,
        EHuman,
        EPolar
    };

}

class CBedDecoratorBase;
//---------------------------------------------------------------------------//
//                       Базовый клас для "кроватей"                         //
//---------------------------------------------------------------------------//
class CBedWidgetBasic : public QWidget
{
    Q_OBJECT
public:
    void     setBedRadius(double);
    inline double   getBedRadius(){return planeRadius;}
    // Преобразование координат
    // 1. Полярные координаты
    QPointF  fromPolarToScreen (double rad,double grad);
    QPointF  fromPolarToPrinter(double rad,double grad);
    QPointF  fromPrinterToPolar(double x,double y);
    QPointF  fromPrinterToPolar(const QPointF& pos){
        return fromPrinterToPolar(pos.x(),pos.y());}
    QPointF  fromScreenToPrinter(double x, double y);
    QPointF  fromScreenToPrinter(const QPointF& pos){
        return fromScreenToPrinter(pos.x(),pos.y());}
    QPointF  fromPrinterToScreen(double x,double y);
    QPointF  fromPrinterToScreen(const QPointF& pos){
        return fromPrinterToScreen(pos.x(),pos.y());}
    QPointF  fromPrinterToHuman(double x,double y);
    QPointF  fromPrinterToHuman(const QPointF& pos){
        return fromPrinterToHuman(pos.x(),pos.y());}
    QPointF  fromHumanToPrinter(double x,double y);
    QPointF  fromHumanToPrinter(const QPointF& pos){
        return fromHumanToPrinter(pos.x(),pos.y());}
    inline double   scaleX(double val){return val*scaleXpriv;}
    inline double   scaleY(double val){return val*scaleYpriv;}
public:
    explicit CBedWidgetBasic(QWidget *parent = 0);
    ~CBedWidgetBasic();
    void          update();
    QSize         sizeHint() const;
protected:
    virtual void  draw(QPainter&){;}
    virtual void  updateUi(){;}
public:
    QRectF          getBedRect(double proc=1.0) const;
protected:
    void            resizeEvent(QResizeEvent * event);
    void            paintEvent (QPaintEvent * event);
    bool            event      (QEvent * pe);
    void            calculateGeometry();
    virtual QSizeF  __getRailSize() const;
    inline QRect    getRectA   ()const {return RectA;}
    inline QRect    getRectB   ()const {return RectB;}
    inline QRect    getRectC   ()const {return RectC;}
    inline QRect    getRectAOpp()const {return RectAOpp;}
    inline QRect    getRectBOpp()const {return RectBOpp;}
    inline QRect    getRectCOpp()const {return RectCOpp;}
private:
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
    double scaleXpriv,scaleYpriv;
    // Украшалки
public:
    bool  addDecorator      (CBedDecoratorBase*);
    void  removeDecorator   (CBedDecoratorBase*dec);
    void  moveDecoratorOnTop(CBedDecoratorBase*dec);
private:
    QList<CBedDecoratorBase*> Decorators;
    void  clearDecorators();
};
//---------------------------------------------------------------------------//
//                                Украшалки                                  //
//---------------------------------------------------------------------------//
class CBedDecoratorBase
{
public:
    friend class CBedWidgetBasic;
    CBedDecoratorBase(CBedWidgetBasic* parent);
    virtual ~CBedDecoratorBase();
    void          moveOnTop();
    virtual void  update();
    void          setVisible(bool fl);
    void          lock() {Locked = true;}
    void          unlock();
protected:
    virtual void  draw(QPainter&){;}
    virtual void  event (QEvent * pe)=0;
protected:
    CBedWidgetBasic*        BedWidget;
    //QPainter                Painter;
    bool                    Visible;
    bool                    Locked;
};


#endif // BED_BASIC_WIDGET_H
