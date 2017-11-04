#ifndef CONTROL_WND_H
#define CONTROL_WND_H

#include <QWidget>
#include <QList>
#include <QPointF>
#include "../../gui/basewnd.h"
#include "../../gui/widgets/beds/black_bed_widget.h"
#include "../../gui/widgets/up_down_widget.h"
#include "../../core/core.h"

namespace Ui {
class control_wnd;
}

class CControlWindow : public CBaseWindow
{
    Q_OBJECT
public:
    explicit CControlWindow(QWidget *parent = 0);
    ~CControlWindow();
    void             updateExtruders();
protected:
    QList<CUpDownWidget*> ExtrudersControl;
    QPointF               cursorPosition;
    void                  updateControls();
    void                  updateCursorPositionControls();
    void                  clearExtruders();
private:
    void                  gotoXYZ(float x,float y,float z);
    void                  shiftXYZ(float x,float y,float z);
private:
    Ui::control_wnd *ui;
    CBlackBedWidget*      BedWidget;
    CUpDownWidget*        XControl;
    CUpDownWidget*        YControl;
    CUpDownWidget*        ZControl;
    QPointF               tmpPoint;
private slots:
    void slotOpened ();
    void slotNewPositionReady (const TVertex& ver);
    void slotReady            (const QString&);
    void slotBusy             (const QString&);
    void slotUpDownControls   (float value);
    void on_pbHomeAll_clicked();
    void on_pbGoXYZ_clicked();
    void slotSwitchCoordinateSystem();
    void slotNewCursorPosition(const QPointF& pos);
    void slotRBMousePress     (const QPointF& pos);
    void slotTab0_goto        (QAction*);
};

#endif // CONTROL_WND_H
