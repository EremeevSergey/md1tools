#include <QPainter>
#include <QBrush>
#include <QFontMetrics>
#include <math.h>
#include <QDebug>
#include <QtNumeric>
#include "radar_decorator.h"

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
CBedRadarDecorator::CBedRadarDecorator(CBedWidgetBasic *parent):CBedVertexDecoratorBase(parent)
{
    ColorGradient.createRainbow(-1,1);
}

CBedRadarDecorator::~CBedRadarDecorator()
{

}

void CBedRadarDecorator::draw(QPainter& painter)
{
    QColor col;
    for (int i=0,n=Vertices.size();i<n;i++){
        TVertex v = Vertices.at(i);
        QPointF c = BedWidget->fromPrinterToScreen(v.X,v.Y);
        if (!qIsNaN(c.x()) && !qIsNaN(c.y())){
            // Круг основания
            if  (qIsNaN(v.Z)) col = QColor(Qt::white);
            else col = ColorGradient.getColorByValue(v.Z*ZScale);
            painter.setPen(col);
            painter.setBrush(QBrush(col));
            painter.drawEllipse(c,3,3);
            if (v.Z!=0 && !qIsNaN(v.Z)){
                // Палка
                QPointF e = QPointF(c.x(),c.y()-v.Z*100*ZScale);
                painter.drawLine(c,e);
                // Палка
                painter.drawLine(e.x()-3,e.y(),e.x()+4,e.y());
            }
        }
    }
}

