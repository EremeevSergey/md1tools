#ifndef HEIGHT_MAP_H
#define HEIGHT_MAP_H

#include <QWidget>
#include <QPaintEvent>
#include <QList>
#include <QRectF>
#include <QPaintEvent>

#include "ui_height_map.h"
#include "../../core/core.h"
#include "../../gui/basewnd.h"
#include "../../gui/widgets/beds/black_bed_widget.h"
#include "../../gui/widgets/beds/radar_decorator.h"
#include "plane_widget.h"

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
class CLegendWidget: public QWidget
{
    Q_OBJECT
public:
    explicit CLegendWidget(QWidget *parent = 0);
    CColorGradient      ColorGradient;
protected:
    void paintEvent(QPaintEvent *event);
};

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
class CHeightMapWindow : public CBaseWindow,public Ui::plane_wnd
{
    Q_OBJECT
public:
    explicit CHeightMapWindow(QWidget *parent = 0);
    ~CHeightMapWindow();
    void    setTestRadius(double rad){RectDecorator->setTestRadius(rad);}
protected:
    CBlackBedWidget*    Bed;
    CPlaneWidget*       RectDecorator;
    CBedRadarDecorator* RadarDecorator;
    int                 currentVertexIndex;
    CLegendWidget*      Legend;
protected:
    bool gotoxyz       ();
    void displayStat   ();
    void updateControls();
    void mainLoop      ();
    void activeLoop    ();
private slots:
    void slotCommandExecuted(int);
    void on_dsbRadius_valueChanged(double arg1);
    void on_dsbHeight_valueChanged(double arg1);
    void on_sbMeshSize_valueChanged(int arg1);
    void on_ZScaleSlider_valueChanged(int value);
    void on_pbSave_clicked();
    void on_pbClear_clicked();
    void on_pbLoad_clicked();
    void on_pbStart_clicked();
    void on_pbStop_clicked();
    void slotRbDecoratorClicked();
    void slotOpened ();
private:
};

#endif // HEIGHT_MAP_H
