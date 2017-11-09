#ifndef CORE_TYPES_H
#define CORE_TYPES_H
#include <QtNumeric>
#include <QString>
#include <QList>

struct TVertex{
    double X;
    double Y;
    double Z;
    TVertex(){X=qQNaN();Y=qQNaN();Z=qQNaN();}
    QString toString(int precision=2){return QString("{%1;%2;%3}").
                                                arg(QString::number(X,'f',precision),
                                                    QString::number(Y,'f',precision),
                                                    QString::number(Z,'f',precision));}

    TVertex(double x,double y){X=x;Y=y;Z=qQNaN();}
    TVertex(double x,double y,double z){X=x;Y=y;Z=z;}
    TVertex& operator=(const TVertex& right) {
            //проверка на самоприсваивание
            if (this == &right) return *this;
            X=right.X;
            Y=right.Y;
            Z=right.Z;
            return *this;
        }
};

class CVertexList:public QList<TVertex>
{
public:
    CVertexList(){;}
};
#endif // CORE_TYPES_H
