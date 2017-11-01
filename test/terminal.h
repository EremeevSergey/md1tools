#ifndef TERMINAL_H
#define TERMINAL_H
#include <QWidget>
#include "ui_terminal.h"
#include "../gui/basewnd.h"
#include "../core/core.h"
#include "../gui/widgets/termwidget.h"
class CTestTerminal : public CBaseWindow,public Ui::terminal
{
    Q_OBJECT

public:
    explicit CTestTerminal(QWidget *parent = 0);
    ~CTestTerminal();
protected:
    CTerminalWidget* terminal;
private slots:
    void  slotNewText(const QString& str);
    void  slotAddLine (CConnection::EDirection direction,
                                  const QString& string);
    void on_pbClear_clicked();

private:
};

#endif // TERMINAL_H
