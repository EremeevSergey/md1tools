#ifndef CORE_TYPES_H
#define CORE_TYPES_H
#include <QtNumeric>

struct TVertex{
    double X;
    double Y;
    double Z;
    TVertex(){X=qQNaN();Y=qQNaN();Z=qQNaN();}
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

#endif // CORE_TYPES_H
