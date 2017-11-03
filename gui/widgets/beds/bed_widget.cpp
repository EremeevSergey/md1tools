#include "bed_widget.h"
#include <math.h>
#include <QSizeF>
#include <QRectF>
#include <QFontMetrics>

//---------------------------------------------------------------------------//
//                                  Кровать                                  //
//---------------------------------------------------------------------------//
CBlackBedWidget::CBlackBedWidget(QWidget *parent) : CBedWidgetBasic(parent)
{
    Transparency = 255;
}

static char strTowerA[] = "Tower A";
static char strTowerB[] = "Tower B";
static char strTowerC[] = "Tower C";

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
    painter.drawRect(RectA);
    painter.drawRect(RectB);
    painter.drawRect(RectC);
    // Названия рельс
    painter.setPen(Qt::white);
    painter.drawText(RectA,strTowerA,Qt::AlignHCenter | Qt::AlignVCenter);
    painter.drawText(RectB,strTowerB,Qt::AlignHCenter | Qt::AlignVCenter);
    painter.drawText(RectC,strTowerC,Qt::AlignHCenter | Qt::AlignVCenter);
}

QSizeF CBlackBedWidget::__getRailSize() const
{

    QFontMetrics fm = fontMetrics();
    QRectF r= fm.boundingRect(strTowerA);
    return QSizeF(r.width()*1.5,r.height()*1.5);
}

void CBlackBedWidget::setTransparency(quint8 tr)
{
    Transparency = tr;
    repaint();
}
