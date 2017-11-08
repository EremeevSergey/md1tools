#ifndef HEIGHT_MAP_H
#define HEIGHT_MAP_H

#include <QWidget>
#include <QPaintEvent>
#include <QList>
#include <QRectF>

#include "ui_plane_wnd.h"
#include "../../core/core.h"
#include "../../gui/basewnd.h"
#include "../../gui/widgets/beds/black_bed_widget.h"
#include "plane_widget.h"

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
    CBlackBedWidget* Bed;
    CPlaneWidget*    RectDecorator;
    int  currentVertexIndex;
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
private:
};

#endif // HEIGHT_MAP_H
