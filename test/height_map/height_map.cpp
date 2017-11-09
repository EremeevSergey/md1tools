#include <QHBoxLayout>
#include <math.h>
#include <QHelpEvent>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include "height_map.h"
#include "../../common.h"

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/

void CHeightMapWindow::slotCommandExecuted(int)
{
    mainLoop();
}

void CHeightMapWindow::on_pbStart_clicked()
{
    if (state==Stoped){
        start();
        state= Active;
        step = GoHome;
        currentVertexIndex = 0;
        Printer.sendGoHomeAll();
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

void CHeightMapWindow::on_pbStop_clicked()
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

void CHeightMapWindow::mainLoop()
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

void CHeightMapWindow::activeLoop()
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
        Printer.sendGetZProbeValue();
        step = ZProbe;
        break;
    case ZProbe:
        ver=RectDecorator->at(currentVertexIndex);
        z = Printer.ZProbe.Z;
        if (!qIsNaN(z)) ver.Z = z - dsbHeight->value();
        else            ver.Z = z;
        RectDecorator->setAt(currentVertexIndex,ver);
        RadarDecorator->setAt(currentVertexIndex,ver);
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


CHeightMapWindow::CHeightMapWindow(QWidget *parent) :
    CBaseWindow(parent),Ui::plane_wnd()
{
    setupUi(this);
    setWindowTitle(tr("Height map."));
    setWindowIcon(QIcon(":/images/heightmap.png"));
    Bed = new CBlackBedWidget();
    RectDecorator  = new CPlaneWidget(Bed);
    RadarDecorator = new CBedRadarDecorator(Bed);
    RadarDecorator->setVisible(false);
    mainLayout->insertWidget(0,Bed,4);
//    mainLayout->addWidget(Bed);

    Legend = new CLegendWidget();
    Legend->ColorGradient.createRainbow(-1,1);
    layoutLegend->insertWidget(1,Legend);

    leCounts->setText(QString::number(RectDecorator->count()));
    dsbRadius->setValue(RectDecorator->getTestRadius());
    sbMeshSize->setValue(RectDecorator->getMeshSize());
    ZScaleSlider->setValue((int)(RectDecorator->getZScale())-1);
    leZScale->setText(QString::number(RectDecorator->getZScale(),'f',1));
    RadarDecorator->set(RectDecorator->getVertices());
    RadarDecorator->setZOffset(RectDecorator->getZOffset());
    RadarDecorator->setZScale(RectDecorator->getZScale());
    connect (&Printer,SIGNAL(signalCommandReady(int)),
             SLOT(slotCommandExecuted(int)),Qt::QueuedConnection);
    connect(rbRectangles,SIGNAL(clicked()),SLOT(slotRbDecoratorClicked()));
    connect(rbRadar     ,SIGNAL(clicked()),SLOT(slotRbDecoratorClicked()));
    connect(&Printer,SIGNAL(signalOpened()),SLOT(slotOpened()));
    updateControls();
}

CHeightMapWindow::~CHeightMapWindow()
{
}

void CHeightMapWindow::updateControls()
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


void CHeightMapWindow::displayStat()
{
    // Statistic
    double max=-1000,min=1000;
    int count=0;
    double sumQ=0,sum=0;
    for (int i=0,n=RectDecorator->count();i<n;i++){
        double v = RectDecorator->at(i).Z;
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
        for (int i=0,n=RectDecorator->count();i<n;i++){
            double v = RectDecorator->at(i).Z;
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

void CHeightMapWindow::on_dsbRadius_valueChanged(double arg1)
{
    RectDecorator->setTestRadius(arg1);
    RadarDecorator->set(RectDecorator->getVertices());
}

void CHeightMapWindow::on_dsbHeight_valueChanged(double arg1)
{
    Q_UNUSED(arg1);
}

bool CHeightMapWindow::gotoxyz()
{
    int count = RectDecorator->count();
    if (count>currentVertexIndex){
        TVertex ver = RectDecorator->at(currentVertexIndex);
        ver.Z = dsbHeight->value();
        step = GoToXYZ;
        Printer.sendGoToXYZ(ver.X,ver.Y,ver.Z);
        return true;
    }
    return false;
}

void CHeightMapWindow::slotOpened ()
{
    CEePromRecord* record;
    record = Printer.EEPROM->at(Printer.EEPROM->indexByStartName("Max printable radius"));
    if (record)
        Bed->setBedRadius(record->FValue);
}

void CHeightMapWindow::on_sbMeshSize_valueChanged(int arg1)
{
    RectDecorator->setMeshSize(arg1);
    leCounts->setText(QString::number(RectDecorator->count()));
    RadarDecorator->set(RectDecorator->getVertices());
}

void CHeightMapWindow::on_ZScaleSlider_valueChanged(int value)
{
    double dval = 1.0 + (double)(value)*0.2;
    RectDecorator ->setZScale(dval);
    RadarDecorator->setZScale(dval);
    leZScale->setText(QString::number(dval,'f',1));
}

void CHeightMapWindow::on_pbClear_clicked()
{
    RectDecorator ->clear();
    RadarDecorator->clear();
}

void CHeightMapWindow::on_pbSave_clicked()
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
                stream << QString("mesh_size: %1")   .arg(QString::number(RectDecorator         ->getMeshSize()  ,'f',3)) << endl;
                stream << QString("max_radius: %1")  .arg(QString::number(RectDecorator         ->getTestRadius(),'f',3)) << endl;
                stream << QString("height: %1")      .arg(QString::number(dsbHeight     ->value()        ,'f',3)) << endl;
                for (int i=0,n=RectDecorator->count();i<n;i++){
                    TVertex vert = RectDecorator->at(i);
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

void CHeightMapWindow::on_pbLoad_clicked()
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
            //RectDecorator->clear();
            //RectDecorator->lock();
            try{
                CVertexList vlist;
                while (!stream.atEnd()){
                    line = stream.readLine().trimmed();
                    QStringList list = line.split(':');
                    if (list.size()>=2){
                        QString name = list.at(0).toLatin1().trimmed();
                        if (name=="mesh_size"){
                            RectDecorator->setMeshSize(list.at(1).trimmed().toDouble());
                            sbMeshSize->setValue(RectDecorator->getMeshSize());
                            RectDecorator->clear();
                        }
                        else if (name=="max_radius"){
                            RectDecorator->setTestRadius(list.at(1).trimmed().toDouble());
                            dsbRadius->setValue(RectDecorator->getTestRadius());
                            RectDecorator->clear();
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
                                vlist.append(vert);
                        }
                    }
                }
                RectDecorator->set(vlist);
                RadarDecorator->set(vlist);
            }
            catch(...){
                mb.setText(file.errorString());
                mb.exec();
            }
            //RectDecorator->unlock();
            displayStat();
        }
        else{
            mb.setText(file.errorString());
            mb.exec();
        }
    }
}


void CHeightMapWindow::slotRbDecoratorClicked()
{
    RectDecorator->setVisible(rbRectangles->isChecked());
    RadarDecorator->setVisible(rbRadar->isChecked());
}

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
CLegendWidget::CLegendWidget(QWidget *parent):QWidget(parent)
{

}

void CLegendWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter;
    float dh = (1.0+1.0)/float(height());// -1.0.....+1.0
    painter.begin(this);
    float h=1;
    for (int i =0,n= height();i<n;i++,h-=dh){
        painter.setPen(ColorGradient.getColorByValue(h));
        painter.drawLine(0,i,width(),i);
    }
    painter.end();
}

