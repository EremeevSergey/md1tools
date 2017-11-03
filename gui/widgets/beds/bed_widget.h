#ifndef BED_WIDGET_H
#define BED_WIDGET_H
#include <QWidget>
#include <QPainter>
#include <QResizeEvent>
#include "bed_basic_widget.h"
//---------------------------------------------------------------------------//
//                                  Кровать                                  //
//---------------------------------------------------------------------------//
class CBlackBedWidget : public CBedWidgetBasic
{
    Q_OBJECT
public:
    explicit CBlackBedWidget(QWidget *parent = 0);
    virtual void  draw(QPainter& painter);
    void          setTransparency(quint8 tr);
private:
    quint8 Transparency;
//    void           __DrawText (QPainter& painter,const QRectF& rect,const QString& name);
    virtual QSizeF  __getRailSize()  const;
};

#endif // BED_WIDGET_H
