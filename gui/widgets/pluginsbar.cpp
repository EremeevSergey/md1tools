#include <QPainter>
#include <QVBoxLayout>
#include <QFontMetrics>
#include "pluginsbar.h"

#define IMAGE_SIZE_X 28
#define IMAGE_SIZE_Y 28

#define BUTTON_SIZE_X 96
#define BUTTON_SIZE_Y 96
//#define IMAGE_SIZE_X 32
//#define IMAGE_SIZE_Y 32

//#define BUTTON_SIZE_X 112
//#define BUTTON_SIZE_Y 112
#define BUTTON_UP_DOWN_SIZE_Y 16

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
CTaskBarSliderButton::CTaskBarSliderButton(QWidget* parent):QFrame(parent)
{
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    underMouse = false;
    resize(BUTTON_SIZE_X,BUTTON_UP_DOWN_SIZE_Y);
}

CTaskBarSliderButton::~CTaskBarSliderButton()
{

}

QSize CTaskBarSliderButton::sizeHint() const
{
    return QSize(BUTTON_SIZE_X,BUTTON_UP_DOWN_SIZE_Y);
}

void CTaskBarSliderButton::setImage(const QIcon& image)
{
    Image = image;
    repaint();
}

void CTaskBarSliderButton::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);
    QFrame::paintEvent(event);
    QPainter painter;
    painter.begin(this);{
        Draw(&painter,rect());
    }
    painter.end();
}

void CTaskBarSliderButton::enterEvent(QEvent * event)
{
    QFrame::enterEvent(event);
    if (isEnabled()) underMouse = true;
    else             underMouse = false;
    repaint();
}

void CTaskBarSliderButton::leaveEvent(QEvent * event)
{
    QFrame::leaveEvent(event);
    underMouse = false;
    repaint();
}

void CTaskBarSliderButton::Draw(QPainter *painter,const QRect& r)
{
    QIcon::Mode mode = QIcon::Disabled;
    QRect d = r;
    if (isEnabled()){
        if (underMouse) {mode = QIcon::Active; d.moveTopLeft(QPoint(d.left()-1,d.top()-1));}
        else            mode = QIcon::Selected;
    }
    Image.paint(painter,d,Qt::AlignCenter,mode,QIcon::On);
}

void CTaskBarSliderButton::mousePressEvent(QMouseEvent * event)
{
    QFrame::mousePressEvent(event);
    if (isEnabled()){
        if (event->button()==Qt::LeftButton)
            emit signalClicked();
        event->accept();
    }
}

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/

int CTaskBarButton::count = 0;

CTaskBarButton::CTaskBarButton(QWidget *parent):CTaskBarSliderButton(parent)
{
    checked = false;
    resize(BUTTON_SIZE_X,BUTTON_SIZE_Y);
    Text = QString("task1_%1").arg(QString::number(count));
    count++;
    setFrameShape(QFrame::NoFrame);
}

CTaskBarButton::~CTaskBarButton()
{
}

void CTaskBarButton::setText(const QString& text)
{
    Text=text;
    setToolTip(Text);
    repaint();
}

QSize CTaskBarButton::sizeHint() const
{
    return QSize(BUTTON_SIZE_X,BUTTON_SIZE_Y);
}

#define LABEL_MARGIN 8
void CTaskBarButton::Draw(QPainter *painter,const QRect& r)
{
    if (Text.isEmpty()) CTaskBarSliderButton::Draw(painter,r);
    else{
        QFont f = painter->font();
        if (underMouse) {
            f.setBold(true);
            painter->setFont(f);
        }
        QFontMetrics fm = painter->fontMetrics();
        int height = fm.height();
        painter->drawText(QRect(LABEL_MARGIN,BUTTON_SIZE_Y-LABEL_MARGIN-height,BUTTON_SIZE_X-LABEL_MARGIN*2,height),
                         Qt::AlignHCenter,
                         fm.elidedText(Text,Qt::ElideRight,BUTTON_SIZE_X-LABEL_MARGIN*2));
        int h2 = BUTTON_SIZE_Y - height - LABEL_MARGIN*2 - LABEL_MARGIN;
        CTaskBarSliderButton::Draw(painter,QRect((BUTTON_SIZE_X-h2)/2,LABEL_MARGIN,h2,h2));
    }
}

void CTaskBarButton::setChecked(bool ch)
{
    if (ch==true){
        setFrameShape(QFrame::Panel);
        setFrameShadow(QFrame::Sunken);
    }
    else{
        setFrameShape(QFrame::NoFrame);
        setFrameShadow(QFrame::Plain);
    }
}

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/

CTaskBar::CTaskBar(QWidget* parent):QWidget(parent)
{
    YOffset = 0;
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    CentralWidget = new QWidget(this);
    ButtonUp      = new CTaskBarSliderButton(this);
    ButtonUp->setImage(QIcon(":/images/pb_up.png"));
    ButtonDown    = new CTaskBarSliderButton(this);
    ButtonDown->setImage(QIcon(":/images/pb_down.png"));
    QVBoxLayout* l= new QVBoxLayout();
    l->setSpacing(0);
    l->setContentsMargins(0,0,0,0);
    l->addWidget(ButtonUp);
    l->addWidget(CentralWidget);
    l->addWidget(ButtonDown);
    setLayout(l);
    connect(ButtonUp  ,SIGNAL(signalClicked()),SLOT(slotClicked()));
    connect(ButtonDown,SIGNAL(signalClicked()),SLOT(slotClicked()));
}

CTaskBar::~CTaskBar()
{

}

QSize CTaskBar::sizeHint() const
{
    return QSize(BUTTON_SIZE_X,BUTTON_UP_DOWN_SIZE_Y*2.5);
}

void CTaskBar::addTask(const QString& name,const QIcon& image)
{
    CTaskBarButton* btn = new CTaskBarButton(CentralWidget);
    btn->setText (name);
    btn->setImage(image);
    Tasks.append(btn);
    connect(btn,SIGNAL(signalClicked()),SLOT(slotClicked()));
}

void CTaskBar::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);
    updateButtons();
}

void CTaskBar::updateButtons()
{
    int totalHeight=Tasks.count()*BUTTON_SIZE_Y;
    bool up = false,down=false;
    if (totalHeight<CentralWidget->height())
        YOffset = 0;
    else{
        if ((totalHeight-YOffset)>CentralWidget->height())
            up = true;
//        else if (YOffset>0)
//            YOffset-=BUTTON_SIZE_Y/2;
        if (YOffset>0)
            down = true;
    }
    ButtonDown  ->setEnabled(up);
    ButtonUp->setEnabled(down);
    for (int i=0;i<Tasks.count();i++)
        Tasks.at(i)->move(0,BUTTON_SIZE_Y*i-YOffset);
}

void CTaskBar::slotClicked()
{
    if (sender()==ButtonDown){
            YOffset+=BUTTON_SIZE_Y/2;
        updateButtons();
    }
    else if (sender()==ButtonUp){
        YOffset-=BUTTON_SIZE_Y/2;
        if (YOffset<0) YOffset=0;
        updateButtons();
    }
    else {
        setActiveTask(Tasks.indexOf((CTaskBarButton*)(sender())));
    }
}

void CTaskBar::setActiveTask(int index)
{
    if (index>=0){
        for (int i=0,n=Tasks.size();i<n;i++){
            CTaskBarButton* but = Tasks.at(i);
            if (but){
                if (i==index){
                    but->setChecked(true);
                    emit signalTaskSelect(but->Text);
                }
                else
                    but->setChecked(false);
            }
        }
    }
}

void CTaskBar::setActiveTask(const QString& name)
{
    for (int i=0,n=Tasks.size();i<n;i++){
        CTaskBarButton* but = Tasks.at(i);
        if (but){
            if (but->Text==name){
                setActiveTask(i);
                break;
            }
        }
    }
}
