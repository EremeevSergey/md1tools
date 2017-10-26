#ifndef LOG_VIEW_H
#define LOG_VIEW_H

#include <QWidget>
#include <QDockWidget>
#include <QString>
#include <QPlainTextEdit>
#include "../core/printer/connection.h"

class CLogView : public QDockWidget
{
    Q_OBJECT
public:
    explicit CLogView(QWidget *parent = 0);
protected:
    QPlainTextEdit* logView;
public slots:
    void slotAddLine(CConnection::EDirection dir,const QString& line);
};

#endif // LOG_VIEW_H
