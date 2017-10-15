#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QScrollArea>
#include "gui/widgets/pluginsbar.h"
#include "gui/log_view.h"
#include "gui/basewnd.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void      addTask      (CBaseWindow* wnd);
    void      setActiveTask(const QString& name=QString());
    bool      isTaskPresent(const QString& name);
protected:
    void      createUi     ();
    void      updateActions();
    void      createActions();
    void      createToolBar();
    void      createMenus  ();
    QAction*  newAction(const QString& name,const char* icon_name);
protected:
    void      showActiveTask(const QString& name);
protected:
    QScrollArea*   centralWindow;
    CLogView*      logView;
//    CTaskSelector* taskSelector;
    CTaskBar*      taskBar;
    QAction*  actOpenPort;
    QAction*  actClosePort;
    QAction*  actSetup;
    QAction*  actReset;
    QString   activeTaskName;
public slots:
    void     slotSetActiveTask(const QString& name);
protected slots:
    void     slotUpdate       ();
    void     slotOpenPort     ();
    void     slotClosePort    ();
    void     slotSetup        ();
    void     slotReset        ();
    void     slotStart        (const QString& task);
    void     slotStop         (const QString& task);
};

#endif // MAINWINDOW_H
