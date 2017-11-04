#include <QFontMetrics>
#include <QPainter>
#include <math.h>
#include "heightmap_widget.h"
#include <QDoubleValidator>
#include <QDebug>
#include <QtNumeric>

//---------------------------------------------------------------------------//
//                      Кровать с величинами всех вершин                     //
//---------------------------------------------------------------------------//
CHeightmapWidget::CHeightmapWidget(QWidget *parent):
    CBedWidgetBasic(parent)
{
    Caption  = new QLabel(this);
    leX      = createLineEdit();
    leX_Opp  = createLineEdit();
    leY      = createLineEdit();
    leY_Opp  = createLineEdit();
    leZ      = createLineEdit();
    leZ_Opp  = createLineEdit();
}

QLineEdit* CHeightmapWidget::createLineEdit()
{
    QLineEdit* le = new QLineEdit(this);
    le->setText("-10.000");
    le->setReadOnly(true);
    le->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    le->setFrame(false);
    le->setMaxLength(7);
    QFontMetrics fm = le->fontMetrics();
    QRectF r= fm.boundingRect(le->text());
    r.moveTopLeft(QPoint(0,0));
    le->setGeometry(r.x(),r.y(),r.width()*1.2,r.height()*1.2);
    return le;
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

void CHeightmapWidget::SetHeights(float X,float Xopp,float Y,float Yopp,float Z,float Zopp)
{
    setHeight(X,leX);
    setHeight(Xopp,leX_Opp);
    setHeight(Y,leY);
    setHeight(Yopp,leY_Opp);
    setHeight(Z,leZ);
    setHeight(Zopp,leZ_Opp);
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
    }
    le->setPalette(palette);
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

void CHeightmapWidget::updateUi   ()
{
    Caption->move(0,0);

    leX->setGeometry    (QRect(getRectA()));
    leY->setGeometry    (QRect(getRectB()));
    leZ->setGeometry    (QRect(getRectC()));
    leX_Opp->setGeometry(QRect(getRectAOpp()));
    leY_Opp->setGeometry(QRect(getRectBOpp()));
    leZ_Opp->setGeometry(QRect(getRectCOpp()));
}

QSizeF CHeightmapWidget::__getRailSize() const
{
    return leX->size();
}

void CHeightmapWidget::draw(QPainter& painter)
{
    QRectF rec = getBedRect();

    QPen pen(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    QPointF a    = fromPolarToScreen(getBedRadius(),-150.0/180.0*M_PI); //120+90=210=-150
    QPointF b    = fromPolarToScreen(getBedRadius(), -30.0/180.0*M_PI); //120+90=210=-150
    QPointF c    = fromPolarToScreen(getBedRadius(),  90.0/180.0*M_PI); //90
    painter.drawLine(a,b);
    painter.drawLine(b,c);
    painter.drawLine(c,a);

    QPointF aopp = fromPolarToScreen(getBedRadius(),  30.0/180.0*M_PI); //30
    QPointF bopp = fromPolarToScreen(getBedRadius(), 150.0/180.0*M_PI); //30
    QPointF copp = fromPolarToScreen(getBedRadius(), -90.0/180.0*M_PI); //-90
    pen.setWidth(1);
    pen.setColor(Qt::gray);
    painter.setPen(pen);
    painter.drawLine(aopp,bopp);
    painter.drawLine(bopp,copp);
    painter.drawLine(copp,aopp);

    pen.setColor(Qt::black);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawArc(rec,0,5760);
}


//---------------------------------------------------------------------------//
//        Кровать с величинами всех вершин для ручной регулировки            //
//---------------------------------------------------------------------------//
CManualHeightmapWidget::CManualHeightmapWidget(QWidget *parent):
    CHeightmapWidget(parent)
{
    showButtons = false;
    autoReadValues = false;
    currentButton = EHomeAll;
    pbX     = createPushButton();
    pbX_Opp = createPushButton();
    pbY     = createPushButton();
    pbY_Opp = createPushButton();
    pbZ     = createPushButton();
    pbZ_Opp = createPushButton();
    pbHomeAll = new QPushButton(this);
    pbHomeAll->setText("Home All");
    QFontMetrics fm = pbHomeAll->fontMetrics();
    QRect r= fm.boundingRect(pbHomeAll->text());
    r.moveTopLeft(QPoint(0,0));
    pbHomeAll->setGeometry(r.x(),r.y(),r.width()*1.2,r.height()*1.2);
    connect(pbHomeAll,SIGNAL(clicked()),SLOT(pbClicked()));

    dsbZOffset = new QDoubleSpinBox(this);
    dsbZOffset->setMinimum(0.0);
    dsbZOffset->setMaximum(100.0);
    dsbZOffset->setValue(20.0);
    dsbZOffset->setSingleStep(0.01);
    dsbZOffset->setDecimals(2);
    dsbZOffset->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);

    connect(leX    ,SIGNAL(editingFinished()),SLOT(textChanged()));
    connect(leX_Opp,SIGNAL(editingFinished()),SLOT(textChanged()));
    connect(leY    ,SIGNAL(editingFinished()),SLOT(textChanged()));
    connect(leY_Opp,SIGNAL(editingFinished()),SLOT(textChanged()));
    connect(leZ    ,SIGNAL(editingFinished()),SLOT(textChanged()));
    connect(leZ_Opp,SIGNAL(editingFinished()),SLOT(textChanged()));

    setEditable(false);
    setShowButtons(false);
}

QPushButton* CManualHeightmapWidget::createPushButton()
{
    QPushButton* pb = new QPushButton(this);
    pb->setText("");
    connect(pb,SIGNAL(clicked()),SLOT(pbClicked()));
    return pb;
}

QSizeF CManualHeightmapWidget::__getRailSize() const
{
    QSizeF s = leX->size();
    return QSizeF(s.width()+s.height(),s.height());
}

void CManualHeightmapWidget::updateUi ()
{
    CHeightmapWidget::updateUi();
    leX->setGeometry    (getLineEditRect(getRectA()));
    leY->setGeometry    (getLineEditRect(getRectB()));
    leZ->setGeometry    (getLineEditRect(getRectC()));
    leX_Opp->setGeometry(getLineEditRect(getRectAOpp()));
    leY_Opp->setGeometry(getLineEditRect(getRectBOpp()));
    leZ_Opp->setGeometry(getLineEditRect(getRectCOpp()));

    pbX->setGeometry    (getPushButtonRect(getRectA()));
    pbY->setGeometry    (getPushButtonRect(getRectB()));
    pbZ->setGeometry    (getPushButtonRect(getRectC()));
    pbX_Opp->setGeometry(getPushButtonRect(getRectAOpp()));
    pbY_Opp->setGeometry(getPushButtonRect(getRectBOpp()));
    pbZ_Opp->setGeometry(getPushButtonRect(getRectCOpp()));

    pbHomeAll->setGeometry(0,height()-leX->height(),pbHomeAll->width(),leX->height());
    dsbZOffset->setGeometry(width()/2  - dsbZOffset->width()/2,
                            height()/2 - dsbZOffset->height()/2,
                            dsbZOffset->width(),dsbZOffset->height());
}

void CManualHeightmapWidget::textChanged()
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

void CManualHeightmapWidget::pbClicked()
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
//                qDebug() << "pbClicked() - checked";
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

void CManualHeightmapWidget::setAutoReadValues(bool val)
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

void CManualHeightmapWidget::on_NewValue(float val)
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

void CManualHeightmapWidget::setEditable(bool b)
{
    leX    ->setReadOnly(!b);
    leX_Opp->setReadOnly(!b);
    leY    ->setReadOnly(!b);
    leY_Opp->setReadOnly(!b);
    leZ    ->setReadOnly(!b);
    leZ_Opp->setReadOnly(!b);
}

void CManualHeightmapWidget::setShowButtons(bool b)
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
