#include "taskselector.h"


CTaskSelector::CTaskSelector(QWidget *parent) :
    QToolBar(parent)
{
    taskComboBox = new QComboBox();
    actStart     = new QAction(QIcon(QString::fromUtf8(":/images/start.png")),tr("Старт"),this);
    actStop      = new QAction(QIcon(QString::fromUtf8(":/images/stop.png")),tr("Стоп"),this);
    connect(actStart,SIGNAL(triggered()),this,SLOT(slotStart()));
    connect(actStop ,SIGNAL(triggered()),this,SLOT(slotStop()));
    addAction(actStart);
    addAction(actStop);
    addWidget(taskComboBox);
    actStop     ->setEnabled(false);
    actStart    ->setEnabled(true);
    taskComboBox->setEnabled(true);
    connect(taskComboBox,SIGNAL(activated(QString)),SLOT(slotTaskSelect(QString)));
}

void CTaskSelector::slotStart ()
{
    actStop     ->setEnabled(true);
    actStart    ->setEnabled(false);
//    taskComboBox->setEnabled(false);
    emit signalStart();
}

void CTaskSelector::slotStop  ()
{
    actStop     ->setEnabled(false);
    actStart    ->setEnabled(true);
//    taskComboBox->setEnabled(true);
    emit signalStop();
}

void CTaskSelector::setEnabled (bool en)
{
    actStop     ->setEnabled(en);
    actStart    ->setEnabled(en);
}


void CTaskSelector::slotTaskSelect  (const QString& task_name)
{
    emit signalTaskSelect(task_name);
}

void CTaskSelector::changeEvent(QEvent* event)
{
    QToolBar::changeEvent(event);
    if (event->type()==QEvent::EnabledChange){
        actStop     ->setEnabled(false);
        actStart    ->setEnabled(true);
//        taskComboBox->setEnabled(true);
    }
}

void CTaskSelector::addTaskName(const QString& task_name)
{
    taskComboBox->addItem(task_name);
}
