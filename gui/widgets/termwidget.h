#ifndef TERMWIDGET_H
#define TERMWIDGET_H
#include <QFrame>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QStringList>
#include "../../core/printer/connection.h"
#include "QKeyEvent"

class CTerminalLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit CTerminalLineEdit(QWidget *parent = 0);
protected:
    QString         last;
    QStringList     history;
    void            keyPressEvent(QKeyEvent *event);
    int             currentPosition;
signals:
    void            signalNewText(const QString& str);
public slots:
};

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
class CTerminalWidget : public QFrame
{
    Q_OBJECT
public:
    explicit CTerminalWidget(QWidget *parent = 0);
    void     clear();
protected:
    QPlainTextEdit*    textEdit;
    CTerminalLineEdit* lineEdit;
signals:
    void  signalNewText(const QString& str);
public slots:
    void  slotAddLine (CConnection::EDirection direction,
                                  const QString& string); // Сигнал Вывода входящих строк в консоль (лог)
};

#endif // TERMWIDGET_H
