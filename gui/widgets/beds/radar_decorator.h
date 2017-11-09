#ifndef RADAR_DECORATOR_H
#define RADAR_DECORATOR_H

#include <QList>
#include <QRectF>
#include <QEvent>
#include <QColor>
#include "../../color_gradient.h"
#include "bed_vertex_decorator_base.h"

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
class CBedRadarDecorator : public CBedVertexDecoratorBase
{
public:
    explicit CBedRadarDecorator(CBedWidgetBasic *parent = 0);
    ~CBedRadarDecorator();
    void    draw(QPainter& painter);
protected:
    void   event            (QEvent* ){;}
    CColorGradient ColorGradient;
};

#endif // RADAR_DECORATOR_H
