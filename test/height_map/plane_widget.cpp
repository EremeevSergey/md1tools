#include <QPainter>
#include <QBrush>
#include <QFontMetrics>
#include <math.h>
#include <QDebug>
#include "plane_widget.h"

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
CPlaneWidget::CPlaneWidget(QWidget *parent):CBlackBedWidget(parent)
{
    ZScale        = 1;
    meshSize      = 12;
    setTestRadius    (70.0);
}

CPlaneWidget::~CPlaneWidget()
{

}

TVertex CPlaneWidget::at(int i)
{
    if (i>=0 && i<Vertices.count())
        return Vertices.at(i);
    return TVertex();
}

void CPlaneWidget::setAt(int i,const TVertex& vertex)
{
    if (i>=0 && i<Vertices.count()){
        Vertices[i]=vertex;
        repaint();
    }
}

void CPlaneWidget::clear()
{
    Vertices.clear();
    repaint();
}

void CPlaneWidget::append(const TVertex& vertex)
{
    Vertices.append(vertex);
    repaint();
}

void CPlaneWidget::setMeshSize(int size)
{
    if (size>1){
        meshSize= size;
        updateVertices();
        repaint();
    }
}

void CPlaneWidget::setTestRadius(double rad)
{
    maxTestRadius = rad;
    updateVertices();
    repaint();
}

void CPlaneWidget::setZScale(double size)
{
    ZScale = size;
    repaint();
}

void CPlaneWidget::updateUi()
{
    CBlackBedWidget::updateUi();
    updateVertices();
}

void CPlaneWidget::clearVertices()
{
    for (int i=0,n=Vertices.size();i<n;i++){
        Vertices[i].Z=qQNaN();
    }
    repaint();
}

double SIN_1=sin(-(M_PI/6.0+M_PI/2.0));
double COS_1=cos(-(M_PI/6.0+M_PI/2.0));

void CPlaneWidget::updateVertices()
{
    double delta = maxTestRadius*2.0/meshSize;
    Vertices.clear();
    for (int x=0;x<meshSize;x++)
        for (int y=0;y<meshSize;y++){
            double dx = -maxTestRadius+(y*(delta)+delta/2.0);
            double dy = +maxTestRadius-(x*(delta)+delta/2.0);
            if (maxTestRadius*maxTestRadius>(dx*dx + dy*dy)){
                QPointF p = fromHumanToPrinter(dx,dy);
                //                Vertices.append(TVertex(dx*COS_1 - dy*SIN_1,dx*SIN_1 + dy*COS_1));
                Vertices.append(TVertex(p.x(),p.y()));
            }
        }



//    double delta = maxTestRadius*2.0/meshSize;
//    Vertices.clear();
//    for (int x=0;x<meshSize;x++)
//        for (int y=0;y<meshSize;y++){
//            double dy = -maxTestRadius+x*(delta)+delta/2.0;
//            double dx = -maxTestRadius+y*(delta)+delta/2.0;
//            if (maxTestRadius*maxTestRadius>(dx*dx + dy*dy)){
//                // Попадаем в круг - добавляем вершину
//                QPointF p = fromPrinterToScreen(dx,dy);
//                Vertices.append(TVertex(p.x(),p.y()));
//            }
//        }
}

bool CPlaneWidget::event (QEvent * pe)
{
    if (pe->type()==QEvent::ToolTip){
        QHelpEvent* he = static_cast<QHelpEvent*>(pe);
        setToolTip("");
        for (int i=0,n=Vertices.size();i<n;i++){
            TVertex ver = Vertices.at(i);
            QRectF r= getVertexRect(ver);
            if (r.contains(he->pos().x(),he->pos().y())){
//                if (!qIsNaN(ver.Z)){
                    setToolTip(ver.toString());
//                }
                break;
            }
        }
    }
    return CBlackBedWidget::event(pe);
}

void CPlaneWidget::draw(QPainter& painter)
{
    CBlackBedWidget::draw(painter);
    QPen pen(Qt::black);
    painter.setPen(pen);
    for (int i=0,n=Vertices.size();i<n;i++){
        painter.save();
        showVertexRect(painter,i);
        painter.restore();
    }
    // Рисуем рамки тестовой окружности
    pen.setWidth(3);
    pen.setColor(Qt::blue);
    painter.setPen(pen);
    painter.drawArc(getBedRect(maxTestRadius/getBedRadius()),0,5760);
}
/*
#define PAINT_OFFSET 5
void CPlaneWidget::showPlane()
{
    QPainter painter;
    painter.begin(this);{
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing,true);
        double size;
        scaleY=1;
        scaleX=1;
        if (width()<height()) {
            size = height();
            scaleX=(double)(width())/size;
        }
        else {
            size = width();
            scaleY=(double)(height())/size;
        }
        painter.scale(scaleX,scaleY);
        calculateGeometry(size);
        painter.save();
            showPrinter(painter);
        painter.restore();
        QPen pen(Qt::black);
        painter.setPen(pen);
        for (int i=0,n=Vertices.size();i<n;i++){
            painter.save();
            showVertexRect(painter,i);
            painter.restore();
        }
        pen.setWidth(3);
        pen.setColor(Qt::blue);
        painter.setPen(pen);
        double s = maxTestRadius*Radius/planeHwRadius;
        painter.drawArc(QRectF(planeCenterX - s,planeCenterY - s,s*2.0,s*2.0),0,5760);
    }
    painter.restore();
    painter.end();
}


*/
void CPlaneWidget::showVertexRect(QPainter &painter, int index)
{
    TVertex ver = Vertices.at(index);
    QString str;
//    QBrush origBrush = painter.brush();
    if (!qIsNaN(ver.Z)){
        painter.setBrush(QBrush(getRectColor(ver.Z)));
        str = QString::number(ver.Z,'f',2);
    }
    else {
        painter.setBrush(QBrush(Qt::white));//origBrush);
    }
    QRectF   vert_rect = getVertexRect(ver);
    painter.drawRect(vert_rect);
//    painter.drawEllipse(vert_rect);
    if (!str.isEmpty()){// Draw Z value
            vert_rect.setLeft  (vert_rect.left  ()+1);
            vert_rect.setTop   (vert_rect.top   ()+1);
            vert_rect.setRight (vert_rect.right ()-1);
            vert_rect.setBottom(vert_rect.bottom()-1);
            painter.drawText(vert_rect,str,Qt::AlignHCenter | Qt::AlignVCenter);
    }
}

#define MAX_PROBE_DEVIATION 2.0

//#define ZERO_COLOR Qt::white
//#define TO_BIG_COLOR Qt::darkGreen
//#define TO_SMALL_COLOR Qt::darkRed
//#define PLUS_COLOR  Qt::green
//#define MINUS_COLOR Qt::red

#define ZERO_COLOR Qt::green
#define TO_BIG_COLOR 101,67,33
#define TO_SMALL_COLOR Qt::darkBlue
#define PLUS_COLOR  101,67,33
#define MINUS_COLOR Qt::blue

QColor CPlaneWidget::getRectColor(double val)
{
    QColor zero(ZERO_COLOR);
    QColor c;
    double max = MAX_PROBE_DEVIATION;
    val = val*ZScale;
    if (val==0) return zero;
    if (val>MAX_PROBE_DEVIATION) return QColor(TO_BIG_COLOR);
    if (val<-MAX_PROBE_DEVIATION) return QColor(TO_SMALL_COLOR);
    else if (val>0) c=QColor(PLUS_COLOR);
    else{
        c    = QColor(MINUS_COLOR);
        max=-max;
    }

    double dr = zero.red  () - c.red();
    double dg = zero.green() - c.green();
    double db = zero.blue () - c.blue();
    dr = zero.red  ()-dr*val/max;
    dg = zero.green()-dg*val/max;
    db = zero.blue ()-db*val/max;

    return QColor(dr,dg,db);
}

QRectF CPlaneWidget::getVertexRect(const TVertex& ver)
{
    float rect_size = maxTestRadius*2.0/meshSize;
    float rx = scaleX(rect_size);
    float ry = scaleY(rect_size);
    QPointF center = fromPrinterToScreen(ver.X,ver.Y);
    return QRectF(center.x()-rx/2.0,center.y()-ry/2.0,rx,ry);
//    return QRectF(ver.X-rx/2.0,ver.Y-ry/2.0,rx,ry);
}

