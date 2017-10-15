#ifndef C2DCHART_H
#define C2DCHART_H
#include <QWidget>
#include <QList>
#include <QPainter>

struct TIntFloatPoint{
    TIntFloatPoint();
    TIntFloatPoint(int x,float y);
    int   X;
    float Y;
    TIntFloatPoint& operator=(const TIntFloatPoint& other) {
        if (this != &other) {
            X =other.X;
            Y =other.Y;
        }
        return *this;
    }
    friend bool operator<(const TIntFloatPoint& l, const TIntFloatPoint& r){
        return l.X<r.X;
    }
};

/*****************************************************************************\
*                                                                             *
\*****************************************************************************/
class C2dChart : public QWidget
{
    Q_OBJECT
public:
    explicit C2dChart(QWidget *parent = 0);
    ~C2dChart();
    QSize  sizeHint() const;
    void   append  (const TIntFloatPoint& value);
protected:
    int                     XMax;
    int                     XStep;
    float                   YMax;
    float                   YStep;
    QList<TIntFloatPoint>   Points;
protected:
    int  XLeft,XRight;
    int  YTop,YBottom;
    void calculateGeometry();
protected:
    void updateUi   (QPainter* painter);
    QPointF getPoint(int i);
    void resizeEvent(QResizeEvent * event);
    void paintEvent (QPaintEvent * event);
};

#endif // C2DCHART_H
