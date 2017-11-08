#ifndef PLANE_WIDGETS_H
#define PLANE_WIDGETS_H

#include <QList>
#include <QRectF>
#include <QEvent>
#include "../../gui/basewnd.h"
#include "../../gui/widgets/beds/bed_vertex_decorator_base.h"

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
class CPlaneWidget : public CBedVertexDecoratorBase
{
public:
    explicit CPlaneWidget(CBedWidgetBasic *parent = 0);
    ~CPlaneWidget();
    void    setTestRadius(double rad);
    double  getTestRadius(){return maxTestRadius;}
    void    setMeshSize(int size);
    int     getMeshSize(){return meshSize;}
    void    draw(QPainter& painter);
protected:
    int    meshSize;
    double maxTestRadius;
protected:
    void   event            (QEvent * pe);
    void   updateVertices   ();
    void   showVertexRect   (QPainter& painter,int index);
    QColor getRectColor     (double val);
    QRectF getVertexRect    (const TVertex& ver);
private:
};

#endif // PLANE_WIDGETS_H
