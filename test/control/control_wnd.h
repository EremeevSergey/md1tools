#ifndef CONTROL_WND_H
#define CONTROL_WND_H


#include <QWidget>
#include <QList>
#include "../../gui/basewnd.h"
#include "../../gui/widgets/bed_widget.h"
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
    CBedWidgetBasic*      BedWidget;
    CUpDownWidget*        XControl;
    CUpDownWidget*        YControl;
    CUpDownWidget*        ZControl;
    QList<CUpDownWidget*> ExtrudersControl;
    void                  clearExtruders();
private:
    Ui::control_wnd *ui;

private slots:
    void slotOpened ();
    void slotNewPositionReady (const TVertex& ver);
    void on_pbHomeAll_clicked();
    void on_pbGoXYZ_clicked();
};

#endif // CONTROL_WND_H
