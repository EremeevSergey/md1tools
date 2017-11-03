#include "up_down_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QIcon>
#include <QtNumeric>
#include <QFont>

CUpDownWidget::CUpDownWidget(const QString &name, QWidget *parent) : QFrame(parent)
{
    setFrameShape (QFrame::Panel);
    setFrameShadow(QFrame::Sunken);
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    step = new QDoubleSpinBox();
    step->setMinimum(0.0);
    step->setMaximum(20.0);
    step->setValue(1.0);
    step->setSingleStep(0.01);
    step->setDecimals(2);
    step->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);

    upButton   = new QToolButton();
    upButton->setIcon(QIcon(":/images/up1.png"));

    downButton = new QToolButton();
    downButton->setIcon(QIcon(":/images/down1.png"));

    labelName  = new QLabel(name);
    labelName->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    labelName->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
//    leY_Opp->setGeometry(QRect(10, 80, 61, 20));
    QFont font = labelName->font();
    font.setBold(true);
    font.setPointSize(font.pointSize()*1.2);
    labelName->setFont(font);

    labelValue      = new QLabel();
    labelValue->setGeometry(step->geometry());
    labelValue->setFrameShape (QFrame::Panel);
    labelValue->setFrameShadow(QFrame::Sunken);
    labelValue->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    labelValue->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    labelValue->setText(tr("---"));

    labelStep = new QLabel();
    font = labelStep->font();
    font.setPointSize(font.pointSize()*0.9);
    labelStep->setFont(font);
    labelStep->setText(tr("step:"));
    labelStep->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);

    QVBoxLayout* v = new QVBoxLayout();
    v->addWidget(labelName,1);

    QHBoxLayout* h = new QHBoxLayout();
    h->addSpacing(2);
    h->addWidget(upButton);
    h->addSpacing(2);

    v->addLayout(h,1);
    v->addWidget(labelValue,1);

    h = new QHBoxLayout();
    h->addSpacing(2);
    h->addWidget(downButton);
    h->addSpacing(2);

    v->addLayout(h,1);
    v->addWidget(labelStep,1);
    v->addWidget(step,1);

    setLayout(v);
}

QSize CUpDownWidget::sizeHint() const
{
//    int w = qMax(
//                qMax(
//                     qMax(
//                          qMax(labelName->width(),upButton->width()),
//                     labelValue->width()),
//                downButton->width()),
//            step->width());
    int h = labelName ->height()+
            upButton  ->height()+
            labelValue->height()+
            downButton->height()+
            step      ->height();

    return QSize(60,h);
}


void CUpDownWidget::setPosition(float pos)
{
    labelValue->setText(QString::number(pos,'f',2));
}
