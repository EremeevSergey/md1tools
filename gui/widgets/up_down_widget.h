#ifndef UP_DOWN_WIDGET_H
#define UP_DOWN_WIDGET_H
#include <QFrame>
#include <QToolButton>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QString>

class CUpDownWidget : public QFrame
{
    Q_OBJECT
public:
    explicit CUpDownWidget(const QString& name,QWidget *parent = 0);
    void     setPosition(float pos);
    void     hidePosition(bool fl);
    QSize    sizeHint() const;
protected:
    QToolButton*    upButton;
    QToolButton*    downButton;
    QLabel*         labelValue;
    QLabel*         labelStep;
    QLabel*         labelName;
    QDoubleSpinBox* step;
signals:
    void            signalUpDown  (float);
public slots:
    void            slotUp  ();
    void            slotDown();
};

#endif // UP_DOWN_WIDGET_H
