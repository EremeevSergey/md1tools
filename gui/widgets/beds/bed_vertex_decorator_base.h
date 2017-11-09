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
class CBedVertexDecoratorBase : public CBedDecoratorBase,public CVertexList
{
public:
    explicit CBedVertexDecoratorBase(CBedWidgetBasic *parent = 0);
    ~CBedVertexDecoratorBase();
    int     count(){return Vertices.count();}
    TVertex at(int i);
    void    setAt(int i,const TVertex& vertex);
    void    clear();
    void    append(const TVertex& vertex);
    void    set(const CVertexList& verts);
    const CVertexList& getVertices(){return Vertices;}

    void    setZScale(double size);
    double  getZScale(){return ZScale;}
    void    setZOffset(double size);
    double  getZOffset(){return ZOffset;}
protected:
    double ZScale;
    double ZOffset;
    CVertexList Vertices;
protected:
    void   updateVertices   ();
private:
};

#endif // BED_VERTEX_DECORATOR_BASE_H
