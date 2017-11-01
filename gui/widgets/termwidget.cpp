#include "termwidget.h"
#include <QVBoxLayout>
#include <QPalette>
#include <QTextCharFormat>
#include <QScrollBar>

CTerminalLineEdit::CTerminalLineEdit(QWidget *parent):QLineEdit(parent)
{
    currentPosition=-1;
}

void CTerminalLineEdit::keyPressEvent(QKeyEvent *event)
{
    int k = event->key();
    if (k==Qt::Key_Up){
        if (currentPosition<(history.size()-1)){
            currentPosition++;
            setText(history.at(currentPosition));
        }
    }
    else if (k==Qt::Key_Down){
        if (currentPosition>0){
            currentPosition--;
            setText(history.at(currentPosition));
        }
        else{
            setText(last);
        }
    }
    else if (k==Qt::Key_Enter || k==Qt::Key_Return){
        QString str = text().trimmed();
        int i = history.indexOf(str);
        if (i>=0) history.removeAt(i);
        history.insert(0,text());
        currentPosition = -1;
        setText("");
        last.clear();
        emit signalNewText(str);
    }
    else {
        QLineEdit::keyPressEvent(event);
        currentPosition = -1;
        last=text();
    }
}

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
CTerminalWidget::CTerminalWidget(QWidget *parent) : QFrame(parent)
{
    textEdit = new QPlainTextEdit();
    textEdit->setFrameShape(QFrame::NoFrame);
    textEdit->setReadOnly(true);
//    textEdit->setPlainText("adasdasdasd\nasdasdasd\nasasdasd\nsdsdf");
//    textEdit->document()->setMaximumBlockCount(100);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    textEdit->setPalette(p);

    QFrame* frame=new QFrame();
    QVBoxLayout* v = new QVBoxLayout();
    v->setContentsMargins(0,0,0,0);
    frame->setLayout(v);
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Plain);
    lineEdit = new CTerminalLineEdit();
    lineEdit->setFrame(false);
    v->addWidget(lineEdit);
    //    lineEdit->setPalette(p);

    v = new QVBoxLayout();
    v->setSpacing(0);
    v->addWidget(textEdit);
    v->addWidget(frame);
    setLayout(v);

    connect(lineEdit,SIGNAL(signalNewText(QString)),SIGNAL(signalNewText(QString)));
}

void CTerminalWidget::slotAddLine (CConnection::EDirection direction,const QString& string)
{
    QTextCharFormat fmt = textEdit->currentCharFormat();
    fmt.setFontFixedPitch(true);
    if (direction==CConnection::Input){
        fmt.setForeground(QBrush(Qt::green));
    }
    else if (direction==CConnection::Output){
        fmt.setForeground(QBrush(Qt::yellow));
    }
    else{
        fmt.clearForeground();
    }
    textEdit->setCurrentCharFormat(fmt);
    textEdit->appendPlainText(string);
    QScrollBar *bar = textEdit->verticalScrollBar();
    bar->setValue(bar->maximum());

}

void CTerminalWidget::clear()
{
    textEdit->clear();
}
