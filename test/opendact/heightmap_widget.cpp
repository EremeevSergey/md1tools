#include <QFontMetrics>
#include <QPainter>
#include <math.h>
#include "heightmap_widget.h"
#include <QDoubleValidator>
#include <QDebug>
#include <QtNumeric>

CHeightmapWidget::CHeightmapWidget(QWidget *parent) :
    QWidget(parent),Ui::Heightmap_widget()
{
    showButtons = false;
    autoReadValues = false;
    currentButton = EHomeAll;
    minSize = size();
    setupUi(this);
    connect(leX    ,SIGNAL(editingFinished()),SLOT(textChanged()));
    connect(leX_Opp,SIGNAL(editingFinished()),SLOT(textChanged()));
    connect(leY    ,SIGNAL(editingFinished()),SLOT(textChanged()));
    connect(leY_Opp,SIGNAL(editingFinished()),SLOT(textChanged()));
    connect(leZ    ,SIGNAL(editingFinished()),SLOT(textChanged()));
    connect(leZ_Opp,SIGNAL(editingFinished()),SLOT(textChanged()));

    connect(pbX      ,SIGNAL(clicked()),SLOT(pbClicked()));
    connect(pbX_Opp  ,SIGNAL(clicked()),SLOT(pbClicked()));
    connect(pbY      ,SIGNAL(clicked()),SLOT(pbClicked()));
    connect(pbY_Opp  ,SIGNAL(clicked()),SLOT(pbClicked()));
    connect(pbZ      ,SIGNAL(clicked()),SLOT(pbClicked()));
    connect(pbZ_Opp  ,SIGNAL(clicked()),SLOT(pbClicked()));
    connect(pbHomeAll,SIGNAL(clicked()),SLOT(pbClicked()));

    setEditable(false);
    setShowButtons(false);
}

CHeightmapWidget::~CHeightmapWidget()
{
}

void CHeightmapWidget::setEditable(bool b)
{
    leX    ->setReadOnly(!b);
    leX_Opp->setReadOnly(!b);
    leY    ->setReadOnly(!b);
    leY_Opp->setReadOnly(!b);
    leZ    ->setReadOnly(!b);
    leZ_Opp->setReadOnly(!b);
}

void CHeightmapWidget::setShowButtons(bool b)
{
    showButtons = b;
    pbX      ->setVisible(b);
    pbX_Opp  ->setVisible(b);
    pbY      ->setVisible(b);
    pbY_Opp  ->setVisible(b);
    pbZ      ->setVisible(b);
    pbZ_Opp  ->setVisible(b);
    pbHomeAll->setVisible(b);
    dsbZOffset->setVisible(b);
}

void CHeightmapWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
}

void CHeightmapWidget::paintEvent(QPaintEvent * event)
{
    QWidget::paintEvent(event);
    QPainter painter;
    painter.begin(this);{
        updateUi(&painter);
    }
    painter.end();
}


QSize CHeightmapWidget::sizeHint() const
{
    return QSize(50,50);//QWidget::sizeHint();
    //    return minSize;
}

void CHeightmapWidget::SetName(const QString& name)
{
    Caption->setText(name);
    QFontMetrics fm = Caption->fontMetrics();
    QRect r= fm.boundingRect(name);
    r.moveTopLeft(QPoint(0,0));
    Caption->setGeometry(r);
    repaint();
}

void CHeightmapWidget::setHeight(float X,QLineEdit* le)
{
    QPalette palette = le->palette();
    if (qIsNaN(X)){
        palette.setColor(QPalette::Base, Qt::white);
        le->setText("-.---");
    }else{
        if      (X<0) palette.setColor(QPalette::Base, Qt::red);
        else if (X>0) palette.setColor(QPalette::Base, Qt::green);
        else          palette.setColor(QPalette::Base, Qt::white);
        le->setText(QString::number(X,'f',3));
//        le->setModified(editable);
    }
    le->setPalette(palette);
}

void CHeightmapWidget::SetHeights(float X,float Xopp,float Y,float Yopp,float Z,float Zopp)
{
    setHeight(X,leX);
    setHeight(Xopp,leX_Opp);
    setHeight(Y,leY);
    setHeight(Yopp,leY_Opp);
    setHeight(Z,leZ);
    setHeight(Zopp,leZ_Opp);
}


#define CIRCLE_OFFSET 2

float cos30 = cos(M_PI/6);
float sin30 = sin(M_PI/6);
float cos60 = cos(M_PI/3);
float sin60 = sin(M_PI/3);
float sqr3  = sqrt(3);
void CHeightmapWidget::updateUi(QPainter* painter)
{
    float w = width();
    float h = height();
    QSizeF sizeLe = getLineEditSize(painter,leX);
    center = QPointF(w/2,h/2);
    pbHomeAll->setGeometry(0,h-sizeLe.height(),pbHomeAll->width(),sizeLe.height());
    dsbZOffset->setGeometry(w/2 - dsbZOffset->width()/2,h/2-dsbZOffset->height()/2,dsbZOffset->width(),dsbZOffset->height());
    float b;
    float x;
    float y;
    if (!showButtons){
        b = (height()-sizeLe.height()*2.0)/2.0;
        x = center.x()-sizeLe.width();
        y = b*cos60;
        leX    ->setGeometry(center.x()-sizeLe.width()/2,0,
                             sizeLe.width(),sizeLe.height());
        leX_Opp->setGeometry(center.x()-sizeLe.width()/2,height()-sizeLe.height(),
                             sizeLe.width(),sizeLe.height());

        leZ_Opp->setGeometry(0,center.y()-y-sizeLe.height(),
                             sizeLe.width(),sizeLe.height());
        leY_Opp->setGeometry(w - sizeLe.width(),center.y()-y-sizeLe.height(),
                             sizeLe.width(),sizeLe.height());
        leY    ->setGeometry(0,center.y()+y,
                             sizeLe.width(),sizeLe.height());
        leZ    ->setGeometry(w - sizeLe.width(),center.y()+y,
                             sizeLe.width(),sizeLe.height());
    }
    else{
        int wid = sizeLe.width()+sizeLe.height();
        b = (height()-sizeLe.height()*2.0)/2.0;
        x = center.x()-wid;
        y = b*cos60;
        leX    ->setGeometry(center.x()-wid/2,0,
                             sizeLe.width(),sizeLe.height());
        leX_Opp->setGeometry(center.x()-wid/2,height()-sizeLe.height(),
                             sizeLe.width(),sizeLe.height());
        pbX    ->setGeometry(center.x()-wid/2+sizeLe.width(),0,
                             sizeLe.height(),sizeLe.height());
        pbX_Opp->setGeometry(center.x()-wid/2+sizeLe.width(),height()-sizeLe.height(),
                             sizeLe.height(),sizeLe.height());

        leZ_Opp->setGeometry(0,center.y()-y-sizeLe.height(),
                             sizeLe.width(),sizeLe.height());
        leY_Opp->setGeometry(w - wid,center.y()-y-sizeLe.height(),
                             sizeLe.width(),sizeLe.height());
        pbZ_Opp->setGeometry(sizeLe.width(),center.y()-y-sizeLe.height(),
                             sizeLe.height(),sizeLe.height());
        pbY_Opp->setGeometry(w - wid + sizeLe.width(),center.y()-y-sizeLe.height(),
                             sizeLe.height(),sizeLe.height());

        leY    ->setGeometry(0,center.y()+y,
                             sizeLe.width(),sizeLe.height());
        leZ    ->setGeometry(w - wid,center.y()+y,
                             sizeLe.width(),sizeLe.height());
        pbY    ->setGeometry(sizeLe.width(),center.y()+y,
                             sizeLe.height(),sizeLe.height());
        pbZ    ->setGeometry(w - wid + sizeLe.width(),center.y()+y,
                             sizeLe.height(),sizeLe.height());

    }
    float a = sqrt(b*b*x*x/(b*b-y*y));
    QRectF rect(center.x()-a +2,center.y()-b +2,a*2 -4,b*2 -4);
    drawCircle(painter,rect);
}

void CHeightmapWidget::drawCircle (QPainter* painter,const QRectF& rec)
{
    QPen pen(Qt::black);
    pen.setWidth(2);
    painter->setPen(pen);
    float x,y,a,b;
    a = rec.width()/2;
    b = rec.height()/2;
    y = b*cos60;
    x = sqrt(a*a*(1-y*y/(b*b)));
    painter->drawLine(rec.left()+a  ,rec.top(),
                      rec.left()+a-x,rec.top()+b+y);
    painter->drawLine(rec.left()+a-x,rec.top()+b+y,
                      rec.left()+a+x,rec.top()+b+y);
    painter->drawLine(rec.left()+a  ,rec.top(),
                      rec.left()+a+x,rec.top()+b+y);
    pen.setWidth(1);
    pen.setColor(Qt::gray);
    painter->setPen(pen);
    painter->drawLine(rec.left()+a  ,rec.top()+b+b,
                      rec.left()+a-x,rec.top()+b-y);
    painter->drawLine(rec.left()+a-x,rec.top()+b-y,
                      rec.left()+a+x,rec.top()+b-y);
    painter->drawLine(rec.left()+a  ,rec.top()+b+b,
                      rec.left()+a+x,rec.top()+b-y);
    pen.setColor(Qt::black);
    pen.setWidth(3);
    painter->setPen(pen);
    painter->drawArc(rec,0,5760);
}

QSizeF CHeightmapWidget::getLineEditSize(QPainter *painter, QLineEdit* le)
{
//    QPainter painter;
    QSizeF s = le->size();
    QFontMetrics fm = painter->fontMetrics();
    s.setWidth(fm.boundingRect(le->text()).width()+12);
    return s;
}

void CHeightmapWidget::textChanged()
{
//    qDebug() << "textChanged()";
    QLineEdit* le = qobject_cast<QLineEdit*>(sender());
    if (le){
        QString str = le->text();
        for (int i=0;i<str.length();i++)
            if (str.at(i)==',') str[i]='.';
        bool   b;
        double f = str.toDouble(&b);
        if (!b) f = qQNaN();
//        qDebug() << f;
        setHeight(f,le);
    }
}

float CHeightmapWidget::getValue(QLineEdit* le)
{
    if (le){
        bool b;
        double f = le->text().toDouble(&b);
        if (!b) f = qQNaN();
        return f;
    }
    return qQNaN();
}

void CHeightmapWidget::pbClicked()
{
    qDebug() << "pbClicked()";
    if      (sender() == pbX)     currentButton = EX;
    else if (sender() == pbX_Opp) currentButton = EXOpp;
    else if (sender() == pbY)     currentButton = EY;
    else if (sender() == pbY_Opp) currentButton = EYOpp;
    else if (sender() == pbZ)     currentButton = EZ;
    else if (sender() == pbZ_Opp) currentButton = EZOpp;
    else
        currentButton=EHomeAll;
    if (!autoReadValues)
        emit pushButtonClicked(currentButton);
    else{
        QPushButton* pb = qobject_cast<QPushButton*>(sender());
        if (pb){
            if (pb->isChecked()){
                if (pbX    !=pb) pbX    ->setChecked(false);
                if (pbX_Opp!=pb) pbX_Opp->setChecked(false);
                if (pbY    !=pb) pbY    ->setChecked(false);
                if (pbY_Opp!=pb) pbY_Opp->setChecked(false);
                if (pbZ    !=pb) pbZ    ->setChecked(false);
                if (pbZ_Opp!=pb) pbZ_Opp->setChecked(false);
                qDebug() << "pbClicked() - checked";
                emit pushButtonClicked(currentButton);
            }
            else{
                currentButton=EHomeAll;
                pbX    ->setChecked(false);
                pbX_Opp->setChecked(false);
                pbY    ->setChecked(false);
                pbY_Opp->setChecked(false);
                pbZ    ->setChecked(false);
                pbZ_Opp->setChecked(false);
                emit pushButtonClicked(currentButton);
            }
        }
    }
}

void CHeightmapWidget::setAutoReadValues(bool val)
{
    autoReadValues = val;
    pbX->setCheckable(autoReadValues);
    pbX->setChecked(false);
    pbX_Opp->setCheckable(autoReadValues);
    pbX_Opp->setChecked(false);
    pbY->setCheckable(autoReadValues);
    pbY->setChecked(false);
    pbY_Opp->setCheckable(autoReadValues);
    pbY_Opp->setChecked(false);
    pbZ->setCheckable(autoReadValues);
    pbZ->setChecked(false);
    pbZ_Opp->setCheckable(autoReadValues);
    pbZ_Opp->setChecked(false);
    currentButton=EHomeAll;
}

void CHeightmapWidget::on_NewValue(float val)
{
    if (autoReadValues){
        if      (currentButton == EX)    setHeight(-val,leX);
        else if (currentButton == EXOpp) setHeight(-val,leX_Opp);
        else if (currentButton == EY)    setHeight(-val,leY);
        else if (currentButton == EYOpp) setHeight(-val,leY_Opp);
        else if (currentButton == EZ)    setHeight(-val,leZ);
        else if (currentButton == EZOpp) setHeight(-val,leZ_Opp);
    }
}
