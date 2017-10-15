#include "basewnd.h"
#include "common.h"

CBaseWindow::CBaseWindow(QWidget *parent) :
    QWidget(parent)
{
    step  = NoStep;
    state = Stoped;
}

void CBaseWindow::start()
{
    emit signalStart(windowTitle());
}

void CBaseWindow::stop ()
{
    emit signalStop(windowTitle());
}

void CBaseWindow::reset()
{
    step  = NoStep;
    state = Stoped;
}



