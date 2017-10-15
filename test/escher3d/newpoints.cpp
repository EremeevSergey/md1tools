#include "newpoints.h"
#include <QDebug>

CTableLine::CTableLine(QWidget *parent) : QWidget(parent)
{
    ButtonVisible = true;
    ButtonCheckable = false;

    XLe = new QLineEdit(this);
    XLe->setGeometry(QRect(170, 10, 51, 20));
    XLe->setReadOnly(true);
    XLe->setAlignment(Qt::AlignCenter);
    XLe->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);

    YLe = new QLineEdit(this);
    YLe->setGeometry(QRect(170, 10, 51, 20));
    YLe->setReadOnly(true);
    YLe->setAlignment(Qt::AlignCenter);
    YLe->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);

    ZLe = new QLineEdit(this);
    ZLe->setGeometry(QRect(170, 10, 51, 20));
    ZLe->setReadOnly(false);
    ZLe->setAlignment(Qt::AlignCenter);
    ZLe->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    BushButton = new QPushButton (this);
    BushButton->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    connect(BushButton,SIGNAL(clicked()),SIGNAL(signalBPCliked()));
    connect(ZLe,SIGNAL(editingFinished()),SLOT(textChanged()));

    setX(0);
    setY(0);
    setZ(qQNaN());
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
}

void CTableLine::setX(float val)
{
    XValue = val;
    XLe->setText(QString::number(val,'f',3));
}

void CTableLine::setY(float val)
{
    YValue = val;
    YLe->setText(QString::number(val,'f',3));
}

void CTableLine::setZ(float val)
{
    ZValue = val;
    QPalette palette = ZLe->palette();
    if (qIsNaN(val)){
        palette.setColor(QPalette::Base, Qt::white);
        ZLe->setText("-.---");
    }else{
        if      (val<0) palette.setColor(QPalette::Base, Qt::red);
        else if (val>0) palette.setColor(QPalette::Base, Qt::green);
        else            palette.setColor(QPalette::Base, Qt::white);
        ZLe->setText(QString::number(val,'f',3));
    }
    ZLe->setPalette(palette);
}

void CTableLine::SetButtonVisible  (bool st)
{
    ButtonVisible = st;
    BushButton->setVisible(st);
    BushButton->setChecked(false);
    Resize();
}

void CTableLine::SetButtonCheckable(bool st)
{
    ButtonCheckable = st;
    BushButton->setCheckable(st);
    BushButton->setChecked(false);
}

QSize CTableLine::sizeHint() const
{
    int i=3;
    if (ButtonVisible) i=4;
    return QSize(XLe->height()*i,XLe->height());
}

void CTableLine::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);
    Resize();
}


#define SPACE 4
void CTableLine::Resize()
{
    int w = size().width();
    int h = XLe->height();
    if (ButtonVisible){
        w-=h+SPACE;
        BushButton->setGeometry(w,0,h,h);
    }
//    qDebug() << XLe->text() << size() << w << h;
    w = (w-2*SPACE)/3;
    XLe->setGeometry(0,0,w,h);
    YLe->setGeometry(w+SPACE,0,w,h);
    ZLe->setGeometry((w+SPACE)*2,0,w,h);
}

void CTableLine::textChanged()
{
    QLineEdit* le = qobject_cast<QLineEdit*>(sender());
    if (le){
        QString str = le->text();
        for (int i=0;i<str.length();i++)
            if (str.at(i)==',') str[i]='.';
        bool   b;
        double f = str.toDouble(&b);
        if (!b) f = qQNaN();
        setZ(f);
    }
}



QTableVertex::QTableVertex(QWidget *parent): QWidget(parent)
{
    MainLayout = new QVBoxLayout(this);
    ButtonCheckable = false;
//    setLayout(MainLayout);
}

QTableVertex::~QTableVertex()
{
    clear();
}

void QTableVertex::clear()
{
    for (int i=0;i<Items.count();i++){
        CTableLine* a = Items.at(i);
        delete a;
    }
    Items.clear();
}

void QTableVertex::setSize(int count)
{
    clear();
    for (int i=0;i<count;i++){
        CTableLine* a = new CTableLine();
        Items.append(a);
        MainLayout->addWidget(a);
        connect(a,SIGNAL(signalBPCliked()),SLOT(slotBPCliked()));
    }
}

void QTableVertex::setX(int pos,float val)
{
    if (pos>=0 && pos<Items.size())
        Items.at(pos)->setX(val);
}

void QTableVertex::setY(int pos,float val)
{
    if (pos>=0 && pos<Items.size())
        Items.at(pos)->setY(val);
}

void QTableVertex::setZ(int pos,float val)
{
    if (pos>=0 && pos<Items.size())
        Items.at(pos)->setZ(val);
}

void QTableVertex::set (int pos,float x,float y,float z)
{
    if (pos>=0 && pos<Items.size()){
       CTableLine* l = Items.at(pos);
       if (l){
           l->setX(x);
           l->setY(y);
           l->setZ(z);
       }
    }
}

float QTableVertex::getX(int pos)
{
    if (pos>=0 && pos<Items.size()) return Items.at(pos)->getX();
    return qQNaN();
}

float QTableVertex::getY(int pos)
{
    if (pos>=0 && pos<Items.size()) return Items.at(pos)->getY();
    return qQNaN();
}

float QTableVertex::getZ(int pos)
{
    if (pos>=0 && pos<Items.size()) return Items.at(pos)->getZ();
    return qQNaN();
}

void QTableVertex::setButtonVisible  (bool st)
{
    for (int i=0;i<Items.size();i++){
        CTableLine* l = Items.at(i);
        if (l) l->SetButtonVisible(st);
    }
}

void QTableVertex::setButtonCheckable(bool st)
{
    for (int i=0;i<Items.size();i++){
        CTableLine* l = Items.at(i);
        if (l) l->SetButtonCheckable(st);
    }
    ButtonCheckable = st;
}

void QTableVertex::uncheckButtons()
{
    for (int i=0;i<Items.size();i++){
        CTableLine* l = Items.at(i);
        if (l) l->uncheckButton();
    }
}

void QTableVertex::slotBPCliked()
{
    CTableLine* s = qobject_cast<CTableLine*>(sender());
    if (s){
        int code=-1;
        for (int i=0;i<Items.size();i++){
            CTableLine* l = Items.at(i);
            if(s==l) code = i;
            else l->uncheckButton();
        }
        if (code>=0) {
            if (!ButtonCheckable)
                emit signalBPCliked(code);
            else if (s->isButtonChecked())
                emit signalBPCliked(code);
        }
    }
}

bool QTableVertex::isAllZValueReady()
{
    for (int i=0;i<Items.size();i++){
        CTableLine* l = Items.at(i);
        if (l && qIsNaN(l->getZ())) return false;
    }
    return true;
}
