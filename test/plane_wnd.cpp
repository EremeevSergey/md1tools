#include <QPainter>
#include <QBrush>
#include <QHBoxLayout>
#include <QFontMetrics>
#include <math.h>
#include <QHelpEvent>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include "plane_wnd.h"
#include "../common.h"

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/

void CPlaneWindow::slotCommandExecuted()
{
    mainLoop();
}

void CPlaneWindow::on_pbStart_clicked()
{
    if (state==Stoped){
        start();
        state= Active;
        step = GoHome;
        currentVertexIndex = 0;
//        Printer.Connection->writeLine("M321");
//        Printer.Connection->writeLine("M322");

        Printer.cmdGoHomeAll();
    }
    else if (state==Active){
        state= WaitPaused;
    }
    else if (state==Paused){
        state= Active;
        mainLoop();
    }
    updateControls();
}

void CPlaneWindow::on_pbStop_clicked()
{
    if (state==Paused){
        state = Stoped;
        stop();
    }
    else{
        state = WaitStop;
    }
    updateControls();
}

void CPlaneWindow::mainLoop()
{
    switch (state) {
    case Active:
        activeLoop();
        break;
    case WaitStop:
        state = Stoped;
        stop();
        break;
    case WaitPaused:
        state = Paused;
        break;
    default:
        break;
    }
    updateControls();
}

void CPlaneWindow::activeLoop()
{
    TVertex ver;
    double z;
    switch (step){
    case GoHome:
        currentVertexIndex = 0;
        if (!gotoxyz()){
            state = Stoped;
            stop();
        }
        break;
    case GoToXYZ:
        Printer.cmdGetZProbeValue();
        step = ZProbe;
        break;
    case ZProbe:
        ver=Plane->at(currentVertexIndex);
        z = Printer.ZProbe.Z;
        if (!qIsNaN(z)) ver.Z = z - dsbHeight->value();
        else            ver.Z = z;
        Plane->setAt(currentVertexIndex,ver);
        displayStat();
        currentVertexIndex++;
        if (!gotoxyz()){
            state = Stoped;
            stop();
        }
        break;
    default:
        break;
    }
}


CPlaneWindow::CPlaneWindow(QWidget *parent) :
    CBaseWindow(parent),Ui::plane_wnd()
{
    setupUi(this);
    setWindowTitle("Height map.");
    setWindowIcon(QIcon(":/images/heightmap.png"));
    Plane = new CPlaneWidget();
    mainLayout->insertWidget(0,Plane,4);
    leCounts->setText(QString::number(Plane->count()));
    dsbRadius->setValue(Plane->getTestRadius());
    sbMeshSize->setValue(Plane->getMeshSize());
    ZScaleSlider->setValue((int)(Plane->getZScale())-1);
    leZScale->setText(QString::number(Plane->getZScale(),'f',1));
    connect (&Printer,SIGNAL(signalCommandExecuted()),this,SLOT(slotCommandExecuted()));
    updateControls();
}

CPlaneWindow::~CPlaneWindow()
{
}

void CPlaneWindow::updateControls()
{
    bool main,bstart,bstop;
    bstart = true;
    bstop  = true;
    main   = false;
    QString str = tr("Start");

    switch (state) {
    case Stoped:
        main  = true;
        bstop = false;
        break;
    case Active:
        str = tr("Pause");
        break;
    case Paused:
        str = tr("Resume");
        break;
    case WaitPaused:
        str = tr("Pause");
        bstop = false;
        bstart = false;
        break;
    case WaitStop:
        str = tr("Pause");
        bstop = false;
        bstart = false;
        break;
    default:
        main = false;
        break;
    }

    leCounts      ->setEnabled(main);
    dsbRadius     ->setEnabled(main);
    dsbHeight     ->setEnabled(main);
    sbMeshSize    ->setEnabled(main);
    pbClear->setEnabled  (main);
    pbSave ->setEnabled   (main);
    pbLoad ->setEnabled   (main);


    pbStart->setEnabled (bstart);
    pbStop ->setEnabled (bstop);
    pbStart->setText    (str);
}


void CPlaneWindow::displayStat()
{
    // Statistic
    double max=-1000,min=1000;
    int count=0;
    double sumQ=0,sum=0;
    for (int i=0,n=Plane->count();i<n;i++){
        double v = Plane->at(i).Z;
        if (!qIsNaN(v)){
            sum+=v;
            if (max<v) max=v;
            if (min>v) min=v;
            count++;
        }
    }
    leMaxValue  ->setText(QString::number(max));
    leMinValue  ->setText(QString::number(min));
    leDelta     ->setText(QString::number(max-min));
    if (count>0){
        double aver = sum/count;
        leAverage->setText(QString::number(aver));
        for (int i=0,n=Plane->count();i<n;i++){
            double v = Plane->at(i).Z;
            if (!qIsNaN(v)){
                sumQ+=(v-aver)*(v-aver);
                count++;
            }
        }
        leErrorValue->setText(QString::number(sqrt(sumQ/count)));
        leAverage   ->setText(QString::number(aver));
    }
    else{
        leErrorValue->setText("");
        leAverage   ->setText("");
    }
}

void CPlaneWindow::on_dsbRadius_valueChanged(double arg1)
{
    Plane->setTestRadius(arg1);
}

void CPlaneWindow::on_dsbHeight_valueChanged(double arg1)
{
    Q_UNUSED(arg1);
}

bool CPlaneWindow::gotoxyz()
{
    int count = Plane->count();
    if (count>currentVertexIndex){
        TVertex ver = Plane->at(currentVertexIndex);
        ver.Z = dsbHeight->value();
        step = GoToXYZ;
        Printer.cmdGoToXYZ(ver.X,ver.Y,ver.Z);
        return true;
    }
    return false;
}

void CPlaneWindow::on_sbMeshSize_valueChanged(int arg1)
{
    Plane->setMeshSize(arg1);
    leCounts->setText(QString::number(Plane->count()));
}

void CPlaneWindow::on_ZScaleSlider_valueChanged(int value)
{
    double dval = 1.0 + (double)(value)*0.2;
    Plane->setZScale(dval);
    leZScale->setText(QString::number(dval,'f',1));
}

void CPlaneWindow::on_pbClear_clicked()
{
    Plane->clearVertices();
}

void CPlaneWindow::on_pbSave_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Select file name"),"",
                                                    tr("Txt-files (*.txt);;All files (*.*)"));
    if (!fileName.isEmpty()){
        QMessageBox mb;
        mb.setIcon(QMessageBox::Critical);
        mb.setInformativeText(tr("Error"));
        mb.setStandardButtons(QMessageBox::Ok);
        mb.setDefaultButton(QMessageBox::Ok);

        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
            QTextStream stream(&file);
            try{
                stream << QString("mesh_size: %1")   .arg(QString::number(Plane         ->getMeshSize()  ,'f',3)) << endl;
                stream << QString("max_radius: %1")  .arg(QString::number(Plane         ->getTestRadius(),'f',3)) << endl;
                stream << QString("height: %1")      .arg(QString::number(dsbHeight     ->value()        ,'f',3)) << endl;
                for (int i=0,n=Plane->count();i<n;i++){
                    TVertex vert = Plane->at(i);
                    stream << QString("x: %1").arg(QString::number(vert.X,'f',3));
                    stream << QString(", y: %1").arg(QString::number(vert.Y,'f',3));
                    if (!qIsNaN(vert.Z))
                        stream << QString(", z: %1").arg(QString::number(vert.Z,'f',3));
                    stream << endl;
                }
            }
            catch(...){
                mb.setText(file.errorString());
                mb.exec();
            }
        }
        else{
            mb.setText(file.errorString());
            mb.exec();
        }
    }
}

void CPlaneWindow::on_pbLoad_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Select file name"),"",
                                                    tr("Txt-files (*.txt);;All files (*.*)"));
    if (!fileName.isEmpty()){
        QMessageBox mb;
        mb.setIcon(QMessageBox::Critical);
        mb.setInformativeText(tr("Error"));
        mb.setStandardButtons(QMessageBox::Ok);
        mb.setDefaultButton(QMessageBox::Ok);

        QString line;
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)){
            QTextStream stream(&file);
            Plane->clear();
            try{
                while (!stream.atEnd()){
                    line = stream.readLine().trimmed();
                    QStringList list = line.split(':');
                    if (list.size()>=2){
                        QString name = list.at(0).toLatin1().trimmed();
                        if (name=="mesh_size"){
                            Plane->setMeshSize(list.at(1).trimmed().toDouble());
                            sbMeshSize->setValue(Plane->getMeshSize());
                            Plane->clear();
                        }
                        else if (name=="max_radius"){
                            Plane->setTestRadius(list.at(1).trimmed().toDouble());
                            dsbRadius->setValue(Plane->getTestRadius());
                            Plane->clear();
                        }
                        else if (name=="height"){
                            dsbHeight->setValue(list.at(1).trimmed().toDouble());
                        }
                        else if (name=="x"){
                            QStringList coord = line.split(',');
                            TVertex vert;
                            for (int i=0,n=coord.size();i<n;i++){
                                list = coord.at(i).split(':');
                                name = list.at(0).toLatin1().trimmed();
                                if      (name=="x") vert.X = list.at(1).trimmed().toDouble();
                                else if (name=="y") vert.Y = list.at(1).trimmed().toDouble();
                                else if (name=="z") vert.Z = list.at(1).trimmed().toDouble();
                            }
                            if (!qIsNaN(vert.X) && !qIsNaN(vert.Y))
                                Plane->append(vert);
                        }
                    }
                }
            }
            catch(...){
                mb.setText(file.errorString());
                mb.exec();
            }
            displayStat();
        }
        else{
            mb.setText(file.errorString());
            mb.exec();
        }
    }
}

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
CPlaneWidget::CPlaneWidget(QWidget *parent):QWidget(parent)
{
    ZScale        = 1;
    meshSize      = 12;
    planeHwRadius = 90.0;
    scaleX        = 1;
    scaleY        = 1;
    calculateGeometry(100);
    setTestRadius    (70.0);
}

CPlaneWidget::~CPlaneWidget()
{

}

TVertex CPlaneWidget::at(int i)
{
    if (i>=0 && i<Vertices.count())
        return Vertices.at(i);
    return TVertex();
}

void CPlaneWidget::setAt(int i,const TVertex& vertex)
{
    if (i>=0 && i<Vertices.count()){
        Vertices[i]=vertex;
        repaint();
    }
}

void CPlaneWidget::clear()
{
    Vertices.clear();
    repaint();
}

void CPlaneWidget::append(const TVertex& vertex)
{
    Vertices.append(vertex);
    repaint();
}

void CPlaneWidget::setMeshSize(int size)
{
    if (size>1){
        meshSize= size;
        updateVertices();
        repaint();
    }
}

void CPlaneWidget::setTestRadius(double rad)
{
    Radius = rad;
    updateVertices();
    repaint();
}

void CPlaneWidget::setZScale(double size)
{
    ZScale = size;
    repaint();
}

void CPlaneWidget::clearVertices()
{
    for (int i=0,n=Vertices.size();i<n;i++){
        Vertices[i].Z=qQNaN();
    }
    repaint();
}

double SIN_1=sin(M_PI/6.0+M_PI/2.0);
double COS_1=cos(M_PI/6.0+M_PI/2.0);

void CPlaneWidget::updateVertices()
{
    double delta = Radius*2.0/meshSize;
    Vertices.clear();
//    double rad = M_PI/6.0+M_PI/2.0;
    for (int x=0;x<meshSize;x++)
        for (int y=0;y<meshSize;y++){
            double dy = -Radius+x*(delta)+delta/2.0;
            double dx = -Radius+y*(delta)+delta/2.0;
//            double dx = -Radius+x*(delta)+delta/2.0;
//            double dy = -Radius+y*(delta)+delta/2.0;
            if (Radius*Radius>(dx*dx + dy*dy)){
                // Попадаем в круг - добавляем вершину
//                Vertices.append(TVertex(dx,dy));
                Vertices.append(TVertex(dx*COS_1 - dy*SIN_1,dx*SIN_1 + dy*COS_1));
//                Vertices.append(TVertex(dx*cos(rad) - dy*sin(rad),dx*sin(rad) + dy*cos(rad)));
////                Vertices.append(TVertex(dx*cos(rad) - dy*sin(rad),dx*sin(rad) + dy*cos(rad),dx*dy/500));
            }
        }
}

bool CPlaneWidget::event (QEvent * pe)
{
    if (pe->type()==QEvent::ToolTip){
        QHelpEvent* he = static_cast<QHelpEvent*>(pe);
        setToolTip("");
        for (int i=0,n=Vertices.size();i<n;i++){
            TVertex ver = Vertices.at(i);
            QRectF r= getVertexRect(ver);
            if (r.contains((double)(he->pos().x())/scaleX,(double)(he->pos().y())/scaleY)){
//                if (!qIsNaN(ver.Z)){
                    setToolTip(QString("{%1,%2,%3}").arg(
                                   QString::number(ver.X,'f',2),
                                   QString::number(ver.Y,'f',2),
                                   QString::number(ver.Z,'f',2)));
//                }
                break;
            }
        }
    }
    return QWidget::event(pe);
}

void CPlaneWidget::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);
    showPlane();
}

#define PAINT_OFFSET 5
void CPlaneWidget::showPlane()
{
    QPainter painter;
    painter.begin(this);{
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing,true);
        double size;
        scaleY=1;
        scaleX=1;
        if (width()<height()) {
            size = height();
            scaleX=(double)(width())/size;
        }
        else {
            size = width();
            scaleY=(double)(height())/size;
        }
        painter.scale(scaleX,scaleY);
        calculateGeometry(size);
        painter.save();
            showPrinter(painter);
        painter.restore();
        QPen pen(Qt::black);
        painter.setPen(pen);
        for (int i=0,n=Vertices.size();i<n;i++){
            painter.save();
            showVertexRect(painter,i);
            painter.restore();
        }
        pen.setWidth(3);
        pen.setColor(Qt::blue);
        painter.setPen(pen);
        double s = planeRadius*Radius/planeHwRadius;
        painter.drawArc(QRectF(planeCenterX - s,planeCenterY - s,s*2.0,s*2.0),0,5760);
    }
    painter.restore();
    painter.end();
}


#define RAIL_SIZE 0.05
void CPlaneWidget::calculateGeometry(double rect_size)
{
    double rail = rect_size*RAIL_SIZE;
    Xa = rect_size/2.0;
    Ya = rail;
    RectA = QRectF(Xa-rail/2.0,0,rail,rail);
    double radius = (rect_size - Ya)/2.0;
    planeRadius = radius-4.0;
    planeCenterX = Xa;
    planeCenterY = Ya+radius;
    double n = radius*cos(M_PI/6.0)*2.0;//cos(30)
    Yb = Ya + n*cos(M_PI/6.0);//cos(30)
    double m = n*sin(M_PI/6.0);
    Xb = Xa - m;
    RectB = QRectF(Xb-rail,Yb,rail,rail);
    Yc = Yb;
    Xc = Xa + m;
    RectC = QRectF(Xc,Yc,rail,rail);
}

void CPlaneWidget::showPrinter(QPainter& painter)
{
    painter.setPen(Qt::black);
    painter.setBrush(QBrush(Qt::black));

    painter.drawEllipse(planeCenterX - planeRadius,planeCenterY - planeRadius,planeRadius*2.0,planeRadius*2.0);

    painter.drawRect(RectA);//A(X)
    painter.drawRect(RectB);//B(Y)
    painter.drawRect(RectC);//C(Z)
    painter.setPen(Qt::white);
    painter.drawText(RectA,"X",Qt::AlignHCenter | Qt::AlignVCenter);//A(X)
    painter.drawText(RectB,"Y",Qt::AlignHCenter | Qt::AlignVCenter);//B(Y)
    painter.drawText(RectC,"Z",Qt::AlignHCenter | Qt::AlignVCenter);//C(Z)
}

#define MAX_PROBE_DEVIATION 2.0

#define ZERO_COLOR Qt::white
#define TO_BIG_COLOR Qt::darkGreen
#define TO_SMALL_COLOR Qt::darkRed
#define PLUS_COLOR  Qt::green
#define MINUS_COLOR Qt::red

void CPlaneWidget::showVertexRect(QPainter &painter, int index)
{
    TVertex ver = Vertices.at(index);
    QString str;
//    QBrush origBrush = painter.brush();
    if (!qIsNaN(ver.Z)){
        painter.setBrush(QBrush(getRectColor(ver.Z)));
        str = QString::number(ver.Z,'f',2);
    }
    else {
        painter.setBrush(QBrush(Qt::white));//origBrush);
    }
    QRectF   vert_rect = getVertexRect(ver);
    painter.drawRect(vert_rect);
//    painter.drawEllipse(vert_rect);
    if (!str.isEmpty()){// Draw Z value
            vert_rect.setLeft  (vert_rect.left  ()+1);
            vert_rect.setTop   (vert_rect.top   ()+1);
            vert_rect.setRight (vert_rect.right ()-1);
            vert_rect.setBottom(vert_rect.bottom()-1);
            painter.drawText(vert_rect,str,Qt::AlignHCenter | Qt::AlignVCenter);
    }
}

QColor CPlaneWidget::getRectColor(double val)
{
    QColor zero(ZERO_COLOR);
    QColor c;
    double max = MAX_PROBE_DEVIATION;
    val = val*ZScale;
    if (val==0) return zero;
    if (val>MAX_PROBE_DEVIATION) return QColor(TO_BIG_COLOR);
    if (val<-MAX_PROBE_DEVIATION) return QColor(TO_SMALL_COLOR);
    else if (val>0) c=QColor(PLUS_COLOR);
    else{
        c    = QColor(MINUS_COLOR);
        max=-max;
    }

    double dr = zero.red  () - c.red();
    double dg = zero.green() - c.green();
    double db = zero.blue () - c.blue();
    dr = zero.red  ()-dr*val/max;
    dg = zero.green()-dg*val/max;
    db = zero.blue ()-db*val/max;

    return QColor(dr,dg,db);
}

double SIN_2=sin(-(M_PI/6.0));
double COS_2=cos(-(M_PI/6.0));

QRectF CPlaneWidget::getVertexRect(const TVertex& ver)
{
    double x,y;
//    double rad;
//    rad = -(M_PI/6.0);//+M_PI/2.0);
//    y= ver.X*cos(rad) - ver.Y*sin(rad);
//    x= ver.X*sin(rad) + ver.Y*cos(rad);
    y= ver.X*COS_2 - ver.Y*SIN_2;
    x= ver.X*SIN_2 + ver.Y*COS_2;

    double rect_size = Radius*2.0/meshSize;
    double scale     = planeRadius/planeHwRadius;
    return  QRectF(planeCenterX+(x-rect_size/2.0)*scale,
                   planeCenterY+(y-rect_size/2.0)*scale,
                   rect_size*scale,rect_size*scale);
}

