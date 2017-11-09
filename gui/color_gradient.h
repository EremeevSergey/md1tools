#ifndef COLOR_GRADIENT_H
#define COLOR_GRADIENT_H
#include <QColor>
#include <QList>

class CColorGradient
{
public:
    CColorGradient();
    void addPoint(double value,QColor color);
    void clear();
    QColor getColorByValue(double value);
    void   createRainbow(double min,double max);
protected:
    QList<double> Values;
    QList<QColor> Colors;
};

#endif // COLOR_GRADIENT_H
