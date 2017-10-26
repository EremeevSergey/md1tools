#ifndef CONTROL_WND_H
#define CONTROL_WND_H

// - M115- Capabilities string

#include <QWidget>
#include "../../gui/basewnd.h"
#include "../../gui/widgets/bed_widget.h"

namespace Ui {
class control_wnd;
}

class CControlWindow : public CBaseWindow
{
    Q_OBJECT

public:
    explicit CControlWindow(QWidget *parent = 0);
    ~CControlWindow();
protected:
    CBedWidgetBasic* BedWidget;
private:
    Ui::control_wnd *ui;
};

#endif // CONTROL_WND_H
