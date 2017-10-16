#ifndef NEWPOINTS_H
#define NEWPOINTS_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QList>
#include <QVBoxLayout>
#include <QtNumeric>

class CTableLine : public QWidget
{
    Q_OBJECT
public:
    explicit CTableLine(QWidget *parent = 0);
    float     getX(){return XValue;}
    float     getY(){return YValue;}
    float     getZ(){return ZValue;}
    void      setX(float val);
    void      setY(float val);
    void      setZ(float val);
    void      SetButtonVisible  (bool st);
    void      SetButtonCheckable(bool st);
    void      uncheckButton(){BushButton->setChecked(false);}
    bool      isButtonChecked(){return BushButton->isChecked();}
    QSize     sizeHint() const;
protected:
    void resizeEvent(QResizeEvent * event);
protected:
    float     XValue;
    float     YValue;
    float     ZValue;
    QLineEdit*   XLe;
    QLineEdit*   YLe;
    QLineEdit*   ZLe;
    QPushButton* BushButton;
    bool         ButtonVisible;
    bool         ButtonCheckable;
    void         Resize();
signals:
    void    signalBPCliked();
protected slots:
    void    textChanged();
};

class QTableVertex:public QWidget
{
    Q_OBJECT
public:
    explicit QTableVertex(QWidget *parent = 0);
    ~QTableVertex();
    void     setSize(int count);
    int      getSize() {return Items.count();}
    void     clear();
    void     setX(int pos,float val);
    void     setY(int pos,float val);
    void     setZ(int pos,float val);
    float    getX(int pos);
    float    getY(int pos);
    float    getZ(int pos);
    void     set (int pos,float x,float y,float z=qQNaN());
    void     setButtonVisible  (bool st);
    void     setButtonCheckable(bool st);
    void     uncheckButtons();
    bool     isAllZValueReady();
protected:
    QList<CTableLine*> Items;
    QVBoxLayout*       MainLayout;
    bool               ButtonCheckable;
signals:
    void    signalBPCliked(int index);
protected slots:
    void    slotBPCliked();
};

#endif // NEWPOINTS_H
