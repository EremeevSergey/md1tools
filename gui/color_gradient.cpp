#include "color_gradient.h"
#include "../core/types.h"
CColorGradient::CColorGradient()
{

}

void CColorGradient::clear()
{
    Values.clear();
    Colors.clear();
}

void CColorGradient::addPoint(double value,QColor color)
{
    int i = Values.indexOf(value);
    if (i>=0)
        Colors[i]=color;
    else{
        for (int i=0,n=Values.count();i<n;i++){
            if (Values.at(i)>value){
                Values.insert(i,value);
                Colors.insert(i,color);
                return;
            }
        }
        Values.append(value);
        Colors.append(color);
    }

}

QColor CColorGradient::getColorByValue(double value)
{
    if (Values.size()<=0) return QColor(Qt::black);
    if (value<=Values.first()) return Colors.first();
    if (value>=Values.last())  return Colors.last();
    // Значение где-то в середине массива - ищемс
    double v1 = Values.first();
    double v2 = Values.first();
    QColor c1 = Colors.first();
    QColor c2 = Colors.first();
    for (int i=1,n=Values.count();i<n;i++){
        if (value<=Values.at(i)){
            c1 = Colors.at(i-1);
            v1 = Values.at(i-1);
            c2 = Colors.at(i);
            v2 = Values.at(i);
            break;
        }
    }
    // Получили цвета, между которыми находится искомый
    v2-=v1;         // - общая длина участка
    v1 =value - v1; // - расстояние до искомой точки
    TVertex t2(c2.red(),c2.green(),c2.blue());
    TVertex t1(c1.red(),c1.green(),c1.blue());
    TVertex dt(t2.X-t1.X,t2.Y-t1.Y,t2.Z-t1.Z);
    double r = t1.X+dt.X*v1/v2;
    double g = t1.Y+dt.Y*v1/v2;
    double b = t1.Z+dt.Z*v1/v2;
    return QColor(r,g,b);
}

void CColorGradient::createRainbow(double min,double max)
{
    double step = (max-min)/7;
    clear();
    addPoint(max       ,QColor(Qt::red))   ;// каждый
    addPoint(max-step*1,QColor(255,128,0)) ;// охотник
    addPoint(max-step*2,QColor(Qt::yellow));// желает
    addPoint(max-step*3,QColor(Qt::green)) ;// знать
    addPoint(max-step*4,QColor(0,191,255)) ;// где
    addPoint(max-step*5,QColor(Qt::blue))  ;// сидит
    addPoint(min       ,QColor(90, 0, 157));// фазан
}
