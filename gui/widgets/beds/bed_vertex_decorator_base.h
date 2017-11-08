#ifndef BED_VERTEX_DECORATOR_BASE_H
#define BED_VERTEX_DECORATOR_BASE_H

#include <QList>
#include <QRectF>
#include <QEvent>
#include "../../../core/core.h"
#include "../../../gui/widgets/beds/bed_basic_widget.h"

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
class CBedVertexDecoratorBase : public CBedDecoratorBase
{
public:
    explicit CBedVertexDecoratorBase(CBedWidgetBasic *parent = 0);
    ~CBedVertexDecoratorBase();
    int     count(){return Vertices.count();}
    TVertex at(int i);
    void    setAt(int i,const TVertex& vertex);
    void    setZScale(double size);
    double  getZScale(){return ZScale;}
    void    setZOffset(double size);
    double  getZOffset(){return ZOffset;}
    void    clear();
    void    append(const TVertex& vertex);
    void    clearVertices();
protected:
    double ZScale;
    double ZOffset;
    QList<TVertex> Vertices;
protected:
    void   updateVertices   ();
private:
};

#endif // BED_VERTEX_DECORATOR_BASE_H
