#ifndef HEIGHTMAP_WIDGET_H
#define HEIGHTMAP_WIDGET_H

#include <QWidget>
#include <QResizeEvent>
#include <QPointF>
#include <QLineEdit>
#include <QLabel>
#include "../../gui/widgets/beds/bed_widget.h"

//---------------------------------------------------------------------------//
//                      Кровать с величинами всех вершин                     //
//---------------------------------------------------------------------------//
class CHeightmapWidget : public CBedWidgetBasic
{
    Q_OBJECT

public:
    explicit CHeightmapWidget(QWidget *parent = 0);
    void   SetName     (const QString& name);
    void   SetHeights  (float X,float Xopp,float Y,float Yopp,float Z,float Zopp);
    float  getX()      {return getValue(leX);}
    float  getXOpp()   {return getValue(leX_Opp);}
    float  getY()      {return getValue(leY);}
    float  getYOpp()   {return getValue(leY_Opp);}
    float  getZ()      {return getValue(leZ);}
    float  getZOpp()   {return getValue(leZ_Opp);}
protected:
    float   getValue(QLineEdit* le);
    void    setHeight(float X,QLineEdit*);
    void    updateUi   ();
    void    draw(QPainter& painter);
    QSizeF  __getRailSize() const;
protected:
    QLabel*         Caption;
    QLineEdit*      leX;
    QLineEdit*      leX_Opp;
    QLineEdit*      leY;
    QLineEdit*      leY_Opp;
    QLineEdit*      leZ;
    QLineEdit*      leZ_Opp;
private:
    QLineEdit*      createLineEdit();
};

//---------------------------------------------------------------------------//
//        Кровать с величинами всех вершин для ручной регулировки            //
//---------------------------------------------------------------------------//
#include <QPushButton>
#include <QDoubleSpinBox>
class CManualHeightmapWidget : public CHeightmapWidget
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
    explicit CManualHeightmapWidget(QWidget *parent = 0);
    void   setEditable(bool);
    void   setShowButtons(bool);
    float  getZOffset(){return dsbZOffset->value();}
    void   setAutoReadValues(bool val);
protected:
    void updateUi   ();
    QSizeF  __getRailSize() const;

    QPushButton*    pbX;
    QPushButton*    pbX_Opp;
    QPushButton*    pbY;
    QPushButton*    pbY_Opp;
    QPushButton*    pbZ;
    QPushButton*    pbZ_Opp;
    QPushButton*    pbHomeAll;
    QDoubleSpinBox* dsbZOffset;

    bool  showButtons;
    bool  autoReadValues;
    EBushButtons currentButton;
private:
    QPushButton*      createPushButton();
    QRect   getLineEditRect  (const QRect& r) const{
            return QRect(r.x(),r.y(),r.width()-r.height(),r.height());}
    QRect   getPushButtonRect(const QRect& r) const{
            return QRect(r.x()+r.width()-r.height(),r.y(),r.height(),r.height());}

signals:
    void    pushButtonClicked(CManualHeightmapWidget::EBushButtons but);
protected slots:
    void    textChanged();
    void    pbClicked  ();
public slots:
    void    on_NewValue(float val);
};
#endif // HEIGHTMAP_WIDGET_H
