#ifndef HEIGHTMAP_WIDGET_H
#define HEIGHTMAP_WIDGET_H

#include <QWidget>
#include <QResizeEvent>
#include <QPointF>
#include "ui_heightmap_widget.h"


class CHeightmapWidget : public QWidget,Ui::Heightmap_widget
{
    Q_OBJECT

public:
    enum EBushButtons{
        EX=0,
        EXOpp,
        EY,
        EYOpp,
        EZ,
        EZOpp,
        EHomeAll
    };
public:
    explicit CHeightmapWidget(QWidget *parent = 0);
    ~CHeightmapWidget();
    QSize  sizeHint() const;
    void   SetName(const QString& name);
    void   SetHeights(float X,float Xopp,float Y,float Yopp,float Z,float Zopp);
    void   setEditable(bool);
    void   setShowButtons(bool);
    float  getX()   {return getValue(leX);}
    float  getXOpp(){return getValue(leX_Opp);}
    float  getY()   {return getValue(leY);}
    float  getYOpp(){return getValue(leY_Opp);}
    float  getZ()   {return getValue(leZ);}
    float  getZOpp(){return getValue(leZ_Opp);}
    float  getZOffset(){return dsbZOffset->value();}
    void   setAutoReadValues(bool val);
protected:
    float  getValue(QLineEdit* le);
    void setHeight(float X,QLineEdit*);
    void resizeEvent(QResizeEvent * event);
    void paintEvent (QPaintEvent * event);
    void updateUi   (QPainter* painter);
    void drawCircle (QPainter* painter,const QRectF& rec);
    QSize minSize;
    bool  showButtons;
    bool  autoReadValues;
    EBushButtons currentButton;

private:
    QSizeF   getLineEditSize(QPainter* painter,QLineEdit* le);
    QPointF center;
    float   radius;
signals:
    void    pushButtonClicked(CHeightmapWidget::EBushButtons but);
protected slots:
    void    textChanged();
    void    pbClicked  ();
public slots:
    void    on_NewValue(float val);
};

#endif // HEIGHTMAP_WIDGET_H
