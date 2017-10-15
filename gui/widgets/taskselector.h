#ifndef TASKSELECTOR_H
#define TASKSELECTOR_H

#include <QToolBar>
#include <QAction>
#include <QToolBox>
#include <QComboBox>
#include <QString>
#include <QEvent>

class CTaskSelector : public QToolBar
{
    Q_OBJECT
public:
    explicit CTaskSelector(QWidget *parent = 0);
    void     addTaskName  (const QString& task_name);
protected:
    void       changeEvent(QEvent* event);
protected:
    QComboBox* taskComboBox;
    QAction*   actStart;
    QAction*   actStop;
signals:
    void       signalStart     ();
    void       signalStop      ();
    void       signalTaskSelect(const QString &task_name);
public slots:
    void       setEnabled      (bool);
    void       slotStart       ();
    void       slotStop        ();
    void       slotTaskSelect  (const QString &task_name);

};

#endif // TASKSELECTOR_H
