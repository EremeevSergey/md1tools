#include <QPainter>
#include <QBrush>
#include <QFontMetrics>
#include <math.h>
#include <QDebug>
#include "bed_vertex_decorator_base.h"

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
CBedVertexDecoratorBase::CBedVertexDecoratorBase(CBedWidgetBasic *parent):
    CBedDecoratorBase(parent)
{
    ZScale        = 1;
    ZOffset       = 0;
}

CBedVertexDecoratorBase::~CBedVertexDecoratorBase()
{

}

TVertex CBedVertexDecoratorBase::at(int i)
{
    if (i>=0 && i<Vertices.count())
        return Vertices.at(i);
    return TVertex();
}

void CBedVertexDecoratorBase::setAt(int i,const TVertex& vertex)
{
    if (i>=0 && i<Vertices.count()){
        Vertices[i]=vertex;
        update();
    }
}

void CBedVertexDecoratorBase::clear()
{
    Vertices.clear();
    update();
}

void CBedVertexDecoratorBase::append(const TVertex& vertex)
{
    Vertices.append(vertex);
    update();
}

void CBedVertexDecoratorBase::set(const CVertexList& verts)
{
    Vertices = verts;
    update();
}

void CBedVertexDecoratorBase::setZScale(double size)
{
    ZScale = size;
    update();
}

void CBedVertexDecoratorBase::setZOffset(double size)
{
    ZOffset = size;
    update();
}

