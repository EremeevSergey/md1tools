#include "mainwindow.h"
#include <QToolBar>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QSplitter>
#include "common.h"
#include "gui/dialogs/setupdlg.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createActions();
    createMenus  ();
    createToolBar();
    createUi     ();
    updateActions();
}

MainWindow::~MainWindow()
{
    Config.WindowsHeight = height();
    Config.WindowsWidth  = width();
    Config.save();
}

void MainWindow::createUi     ()
{
    centralWindow=new QScrollArea(this);
    centralWindow->setLayout(new QVBoxLayout());
    centralWindow->setEnabled(true);
    QHBoxLayout* lay = new QHBoxLayout();
    QFrame* frame = new QFrame();
    frame->setFrameShape(QFrame::Panel);
    frame->setFrameShadow(QFrame::Sunken);

    lay->addWidget(frame);
    lay->addWidget(centralWindow);
    QWidget* w = new QWidget();
    w->setLayout(lay);
    taskBar = new CTaskBar();
    lay = new QHBoxLayout();
    lay->setSpacing(0);
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(taskBar);
    frame->setLayout(lay);
    setCentralWidget(w);
    logView = new CLogView(this);
    addDockWidget(Qt::BottomDockWidgetArea,logView);
    logView->hide();
    connect(Printer.Connection,SIGNAL(signalAddToLog(CConnection::EDirection,QString)),
            logView,SLOT(slotAddLine(CConnection::EDirection,QString)));
    connect(logView,SIGNAL(visibilityChanged(bool)),SLOT(slotLogClosed(bool)));
    connect(taskBar,SIGNAL(signalTaskSelect(QString)),this,SLOT(slotSetActiveTask(QString)));
    connect(Printer.Connection,SIGNAL(signalOpened()),this,SLOT(slotUpdate()));
    connect(Printer.Connection,SIGNAL(signalClosed()),this,SLOT(slotUpdate()));
}

void MainWindow::createActions()
{
    actOpenPort  = newAction(tr("Connect"),":/images/open.png");
    actClosePort = newAction(tr("Diconnect" ),":/images/close.png");
    actSetup     = newAction(tr("Setup"   ),":/images/key1.png");
    actReset     = newAction(tr("Reset"   ),":/images/stop_1.png");
    actLogView   = newAction(tr("Log"   ),":/images/text-x-log.png");
    actLogView->setCheckable(true);
    actLogView->setChecked(false);

    connect(actOpenPort ,SIGNAL(triggered()),this,SLOT(slotOpenPort ()));
    connect(actClosePort,SIGNAL(triggered()),this,SLOT(slotClosePort()));
    connect(actSetup    ,SIGNAL(triggered()),this,SLOT(slotSetup    ()));
    connect(actReset    ,SIGNAL(triggered()),this,SLOT(slotReset    ()));
    connect(actLogView  ,SIGNAL(triggered()),this,SLOT(slotLog      ()));
}

void MainWindow::createToolBar()
{
    QToolBar* tb = new QToolBar(tr("Главное"));
    tb->addAction(actOpenPort);
    tb->addAction(actClosePort);
    tb->addSeparator();
    tb->addAction(actReset);
    tb->addSeparator();
    tb->addAction(actLogView);
    tb->addSeparator();
    tb->addAction(actSetup);
    addToolBar(Qt::TopToolBarArea,tb);
}

void MainWindow::createMenus()
{
    QMenu* pmenu = new QMenu(tr("Главное"));
    pmenu->addAction(actOpenPort);
    pmenu->addAction(actClosePort);
    pmenu->addSeparator();
    pmenu->addAction(actSetup);
    pmenu->addSeparator();
    pmenu->addAction(tr("Exit"),qApp,SLOT(closeAllWindows()),QKeySequence("CTRL+Q"));
//    menuBar()->addMenu(pmenu);
}

QAction* MainWindow::newAction(const QString &name, const char* icon_name)
{
    return new QAction(QIcon(QString::fromUtf8(icon_name)),name,this);
}


void MainWindow::updateActions()
{
    bool fl = Printer.Connection->isOpened();
    actOpenPort->setEnabled(!fl);
    actClosePort->setEnabled(fl);
//    centralWindow->setEnabled(fl);
}

void MainWindow::slotUpdate   ()
{
    updateActions();
}

void MainWindow::slotOpenPort ()
{
    Printer.Connection->open();
}

void MainWindow::slotClosePort()
{
    Printer.Connection->close();
}

void MainWindow::slotSetup    ()
{
    CSetupDialog dlg;
    dlg.setSerialPortName(Config.PortName);
    dlg.setSerialBaudRate(Config.BaudRate);
    if (dlg.exec()==QDialog::Accepted){
        Config.PortName = dlg.getSerialPortName();
        Config.BaudRate = dlg.getSerialBaudRate();
        Printer.Connection->setSerialPortName(Config.PortName);
        Printer.Connection->setSerialPortBaudRate(Config.BaudRate);
    }
}

void MainWindow::addTask (CBaseWindow* wnd)
{
    if (wnd){
        centralWindow->layout()->addWidget(wnd);
        taskBar->addTask(wnd->windowTitle(),wnd->windowIcon());
        connect (wnd,SIGNAL(signalStart(QString)),SLOT(slotStart(QString)));
        connect (wnd,SIGNAL(signalStop (QString)),SLOT(slotStop (QString)));
    }
}

void MainWindow::setActiveTask (const QString& name)
{
    if (!name.isEmpty())
        taskBar->setActiveTask(name);
    else
        taskBar->setActiveTask();
}

void MainWindow::slotSetActiveTask (const QString& name)
{
    showActiveTask (name);
}

void MainWindow::showActiveTask(const QString& name)
{
    int count = centralWindow->layout()->count();
    if (count){
        QString act_task;
        for (int i=0;i<count;i++){
            QWidget* wid = centralWindow->layout()->itemAt(i)->widget();
            if (wid) {
                if (wid->windowTitle()==name){
                    act_task = name;
                    wid->setVisible(true);
                }
                else wid->setVisible(false);
                bool fl = (!activeTaskName.isEmpty() && wid->windowTitle()==activeTaskName) ||
                           (activeTaskName.isEmpty() && wid->windowTitle()==act_task);
                wid->setEnabled(fl);
            }
        }
    }
}

void MainWindow::slotStart (const QString& task)
{
    if (isTaskPresent(task))
        activeTaskName = task;
    int count = centralWindow->layout()->count();
    if (count){
        for (int i=0;i<count;i++){
            QWidget* wid = centralWindow->layout()->itemAt(i)->widget();
            if (wid)
                wid->setEnabled(wid->windowTitle()==task);
        }
    }
}

void MainWindow::slotStop (const QString &task)
{
    if (isTaskPresent(task)){
        activeTaskName.clear();
        showActiveTask(task);
    }
    else
        showActiveTask(activeTaskName);
}

bool MainWindow::isTaskPresent(const QString& name)
{
    if (!name.isEmpty()){
        int count = centralWindow->layout()->count();
        for (int i=0;i<count;i++){
            QWidget* wid = centralWindow->layout()->itemAt(i)->widget();
            if (wid && wid->windowTitle()==name) return true;
        }
    }
    return false;
}

void MainWindow::slotReset()
{
    Printer.sendEmergencyReset();
    Printer.Connection->close();
    int count = centralWindow->layout()->count();
    if (count){
        for (int i=0;i<count;i++){
            CBaseWindow* wid = qobject_cast<CBaseWindow*>(centralWindow->layout()->itemAt(i)->widget());
            if (wid)
                wid->reset();
        }
    }
    Printer.Connection->open();
}

void MainWindow::slotLog()
{
    logView->setVisible(actLogView->isChecked());
}

void MainWindow::slotLogClosed(bool fl)
{
    if (!fl)
        actLogView->setChecked(false);
}
