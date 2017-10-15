#ifndef TASK_BAR_H
#define TASK_BAR_H
#include <QFrame>
#include <QList>
#include <QStringList>
#include <QIcon>
#include <QRect>
#include <QPaintEvent>
#include <QSize>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPainter>

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
class  CTaskBarSliderButton : public QFrame
{
    Q_OBJECT
public:
    CTaskBarSliderButton(QWidget* parent=0);
    ~CTaskBarSliderButton();
    void setImage(const QIcon &image);
    QSize sizeHint() const;
protected:
    bool      underMouse;
    QIcon     Image;
    virtual void Draw(QPainter* painter, const QRect &rect);
protected:
    void      enterEvent     (QEvent * event);
    void      leaveEvent     (QEvent * event);
    void      paintEvent     (QPaintEvent * event);
    void      mousePressEvent(QMouseEvent * event);
signals:
    void      signalClicked();
};
/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
class  CTaskBar;
class  CTaskBarButton : public CTaskBarSliderButton
{
    Q_OBJECT
public:
    friend class CTaskBar;
    CTaskBarButton(QWidget* parent=0);
    ~CTaskBarButton();
    void setText (const QString& text);
    QSize sizeHint() const;
    void       setChecked(bool);
protected:
    bool       checked;
    static int count;
    QString    Text;
protected:
    virtual void Draw(QPainter* painter, const QRect &rect);
};

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
class  CTaskBar : public QWidget
{
    Q_OBJECT
public:
    CTaskBar(QWidget* parent=0);
    ~CTaskBar();
    QSize sizeHint() const;
    void addTask      (const QString& name,const QIcon& image);
    void setActiveTask(const QString& name);
    void setActiveTask(int index = 0);
protected:
    QList<CTaskBarButton*> Tasks;
    QWidget*               CentralWidget;
    CTaskBarSliderButton*  ButtonUp;
    CTaskBarSliderButton*  ButtonDown;
    int                    YOffset;
protected:
    void updateButtons();
    void resizeEvent(QResizeEvent * event);
protected slots:
    void slotClicked();
signals:
    void signalTaskSelect(const QString &task_name);
};

#endif // TASK_BAR_H
