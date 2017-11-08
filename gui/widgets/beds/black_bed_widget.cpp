#include "black_bed_widget.h"
#include <math.h>
#include <QSizeF>
#include <QRectF>
#include <QFontMetrics>
#include <QtNumeric>
#include <QDebug>

//---------------------------------------------------------------------------//
//                                  Кровать                                  //
//---------------------------------------------------------------------------//
CBlackBedWidget::CBlackBedWidget(QWidget *parent) : CBedWidgetBasic(parent)
{
    Transparency = 255;
    cursorVisible = false;
    cursorPosition = QPointF(qQNaN(),qQNaN());
    hotEndVisible = false;
    hotEndPosition = QPointF(qQNaN(),qQNaN());
}

static char strTowerA[] = "Tower A";
static char strTowerB[] = "Tower B";
static char strTowerC[] = "Tower C";

#define CURSOR_SIZE 20
#define HOT_END_SIZE 22

void CBlackBedWidget::draw(QPainter& painter)
{
    painter.setRenderHint(QPainter::Antialiasing,true);
    Qt::GlobalColor col = Qt::black;
    if (!isEnabled()) col = Qt::darkGray;
    QColor color(col);
    color.setAlpha(Transparency);
    painter.setPen(col);
    painter.setBrush(QBrush(color));
    // Рисуем элипс кровати
    painter.drawEllipse(getBedRect());
    // Прямоугольники рельс
    painter.setBrush(QBrush(col));
    painter.drawRect(getRectA());
    painter.drawRect(getRectB());
    painter.drawRect(getRectC());
    // Названия рельс
    painter.setPen(Qt::white);
    painter.drawText(getRectA(),strTowerA,Qt::AlignHCenter | Qt::AlignVCenter);
    painter.drawText(getRectB(),strTowerB,Qt::AlignHCenter | Qt::AlignVCenter);
    painter.drawText(getRectC(),strTowerC,Qt::AlignHCenter | Qt::AlignVCenter);
    // LCD
    QRectF lcd = getRectAOpp();
    double x = lcd.x();
    double w = lcd.width();
    lcd.setLeft (x-2*w);
    lcd.setRight(x+3*w);
    lcd.setHeight(lcd.height()*0.5);
    painter.drawRect(lcd);
    // Курсор
    if (cursorVisible && !qIsNaN(cursorPosition.x()) && !qIsNaN(cursorPosition.y())){
        QPointF p = fromPrinterToScreen(cursorPosition);
        painter.setPen(Qt::green);
        painter.drawLine(p.x()-CURSOR_SIZE/2.0,p.y()                ,p.x()+CURSOR_SIZE/2.0,p.y());
        painter.drawLine(p.x()                ,p.y()-CURSOR_SIZE/2.0,p.x()                ,p.y()+CURSOR_SIZE/2.0);
    }

    // Hotend
    if (hotEndVisible && !qIsNaN(hotEndPosition.x()) && !qIsNaN(hotEndPosition.y())){
        QPointF p = fromPrinterToScreen(hotEndPosition);
        painter.setPen(Qt::red);
        painter.drawLine(p.x()-HOT_END_SIZE/2.0,p.y(),p.x()+HOT_END_SIZE/2.0,p.y());
        painter.drawLine(p.x(),p.y()-HOT_END_SIZE/2.0,p.x(),p.y()+HOT_END_SIZE/2.0);
        painter.drawArc(QRectF(p.x()-HOT_END_SIZE/6.0,p.y()-HOT_END_SIZE/6.0,HOT_END_SIZE/3.0,HOT_END_SIZE/3.0),0,5760);
    }

    // Оси
//    QPointF p1,p2,p3,p4;
//    p1 = fromPrinterToScreen(fromHumanToPrinter(-80,0));
//    p2 = fromPrinterToScreen(fromHumanToPrinter(+80,0));
//    p3 = fromPrinterToScreen(fromHumanToPrinter(0,-80));
//    p4 = fromPrinterToScreen(fromHumanToPrinter(0,+80));
//    painter.drawLine(p1,p2);
//    painter.drawLine(p2,QPointF(p2.x(),p2.y()+10));
//    painter.drawLine(p3,p4);
//    painter.drawLine(p4,QPointF(p4.x()+10,p4.y()));
}

QSizeF CBlackBedWidget::__getRailSize() const
{

    QFontMetrics fm = fontMetrics();
    QRectF r= fm.boundingRect(strTowerA);
    return QSizeF(r.width()*1.3,r.height()*1.9);
}

void CBlackBedWidget::setTransparency(quint8 tr)
{
    Transparency = tr;
    repaint();
}

void CBlackBedWidget::setCursorVisible(bool fl)
{
    if (cursorVisible != fl){
        setMouseTracking(fl);
        cursorVisible = fl;
        repaint();
    }
}

QPointF CBlackBedWidget::setCursorPosition(const QPointF& pos)
{
//    qDebug() << "     " <<pos;
    double rad = getBedRadius();
    if ((rad*rad)>=(pos.x()*pos.x()+pos.y()*pos.y())){
        cursorPosition = pos;
        emit signalNewCursorPosition (pos);
    }
    else{
        cursorPosition = QPointF(qQNaN(),qQNaN());
    }
    if (cursorVisible) repaint();
    return cursorPosition;
}

void CBlackBedWidget::mouseMoveEvent(QMouseEvent *event)
{
    CBedWidgetBasic::mouseMoveEvent(event);
    if (cursorVisible){
        setCursorPosition(fromScreenToPrinter(event->pos().x(),event->pos().y()));
    }
}

void CBlackBedWidget::mousePressEvent(QMouseEvent *event)
{
    CBedWidgetBasic::mouseMoveEvent(event);
    if (cursorVisible && (event->button() & Qt::RightButton)){
        emit signalRBMousePress(fromScreenToPrinter(event->pos().x(),event->pos().y()));
    }
}

void    CBlackBedWidget::setHotEndVisible (bool fl)
{
    if (hotEndVisible != fl){
        hotEndVisible = fl;
        repaint();
    }
}

QPointF CBlackBedWidget::setHotEndPosition(const QPointF& pos)
{
    double rad = getBedRadius();
    if ((rad*rad)>=(pos.x()*pos.x()+pos.y()*pos.y())){
        hotEndPosition = pos;
    }
    else{
        hotEndPosition = QPointF(qQNaN(),qQNaN());
    }
    if (hotEndVisible) repaint();
    return hotEndPosition;
}
