#include "log_view.h"
#include <QVBoxLayout>
#include <QScrollBar>
#include <QTextCharFormat>
#include "../core/core.h"

CLogView::CLogView(QWidget *parent) :
    QDockWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,false);
    QWidget* centralWidget = new QWidget();
    QVBoxLayout* lay = new QVBoxLayout(centralWidget);
    logView = new QPlainTextEdit();
    lay->addWidget(logView);
    setWidget(centralWidget);
}


void CLogView::slotAddLine(const QString& line)
{
    CConnection::Direction dir;
    QString str = CConnection::logStringToString(line,&dir);
    if (str.startsWith("wait") || str.startsWith("busy")) return;

    QTextCharFormat fmt = logView->currentCharFormat();
    fmt.setFontFixedPitch(false);
    if (dir==CConnection::Input){
        fmt.setForeground(QBrush(Qt::red));
    }
    else if (dir==CConnection::Output){
        fmt.setForeground(QBrush(Qt::blue));
    }
    else{
        fmt.clearForeground();
    }
    logView->setCurrentCharFormat(fmt);
    logView->appendPlainText(str);
    QScrollBar *bar = logView->verticalScrollBar();
    bar->setValue(bar->maximum());
}


