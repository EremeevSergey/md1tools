#ifndef PLANE_WIDGETS_H
#define PLANE_WIDGETS_H

#include <QList>
#include <QRectF>
#include "../../core/core.h"
#include "../../gui/basewnd.h"
#include "../../gui/widgets/beds/black_bed_widget.h"

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
class CPlaneWidget : public CBlackBedWidget
{
    Q_OBJECT
public:
    explicit CPlaneWidget(QWidget *parent = 0);
    ~CPlaneWidget();
    void    setTestRadius(double rad);
    double  getTestRadius(){return maxTestRadius;}
    int     count(){return Vertices.count();}
    TVertex at(int i);
    void    setAt(int i,const TVertex& vertex);
    void    setMeshSize(int size);
    int     getMeshSize(){return meshSize;}
    void    setZScale(double size);
    double  getZScale(){return ZScale;}
    void    clear();
    void    append(const TVertex& vertex);
    void    clearVertices();
protected:
    double ZScale;
    int meshSize;
    QList<TVertex> Vertices;
    double maxTestRadius;
protected:
    void   draw(QPainter& painter);
    bool   event            (QEvent * pe);
    void   updateUi();
    void   updateVertices   ();
    void   showVertexRect   (QPainter& painter,int index);
    QColor getRectColor     (double val);
    QRectF getVertexRect    (const TVertex& ver);
private:
};

#endif // PLANE_WIDGETS_H
