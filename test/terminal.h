#ifndef TERMINAL_H
#define TERMINAL_H
#include <QWidget>
#include "ui_terminal.h"
#include "../gui/basewnd.h"
#include "../core/core.h"
#include "../common.h"

class CTestTerminal : public CBaseWindow,public Ui::terminal
{
    Q_OBJECT

public:
    explicit CTestTerminal(QWidget *parent = 0);
    ~CTestTerminal();
protected:
private slots:
    void on_pushButton_clicked();

private:
};

#endif // TERMINAL_H
