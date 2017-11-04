#ifndef BLACK_BED_WIDGET_H
#define BLACK_BED_WIDGET_H
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
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
    void          setTransparency  (quint8 tr);
    void          setCursorVisible (bool);
    QPointF       setCursorPosition(const QPointF& pos);
    void          setHotEndVisible (bool);
    QPointF       setHotEndPosition(const QPointF& pos);
protected:
    void          mouseMoveEvent(QMouseEvent *event);
    void          mousePressEvent(QMouseEvent *event);
private:
    quint8  Transparency;
    QPointF cursorPosition;
    QPointF hotEndPosition;
    bool    cursorVisible;
    bool    hotEndVisible;
    virtual QSizeF  __getRailSize()  const;
signals:
    void          signalNewCursorPosition(const QPointF& pos);
    void          signalRBMousePress(const QPointF& pos);
};

#endif // BLACK_BED_WIDGET_H
