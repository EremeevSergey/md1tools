#ifndef ESCHER3D_WND_H
#define ESCHER3D_WND_H
#include <QAbstractTableModel>
#include <QStringList>
#include <QModelIndex>
#include "escher3d.h"
#include "newpoints.h"
#include "../../core/core.h"

#include "ui_escher3d_wnd.h"
#include "../../gui/basewnd.h"

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
class CEscher3dWindow : public CBaseWindow, public Ui::escher3d_wnd
{
    Q_OBJECT

protected:
    void    calc();
    void    calcProbePoints();
    void    copyToInitial  ();
    void    getParameters();
    void    convertIncomingEndstops();
    void    convertOutgoingEndstops();
    void    setNewParameters();
    void    generateCommands();
    QString DoDeltaCalibration();
protected:
    bool             __normalise;
    CDeltaParameters deltaParams;
    double           __bedRadius;
    int              __numPoints;
    int              __numFactors;
    QList<TVertex>   BedProbePoints;
    QStringList      Debug;
    QTableVertex*    TableVertex;

protected:
    int  currectVertexIndex;
    bool gotoxyz       ();
    void updateControls();
    void mainLoop      ();
    void activeLoop    ();

private slots:
    void slotCommandExecuted(int);

public:
    explicit CEscher3dWindow(QWidget *parent = 0);
    ~CEscher3dWindow();
protected:
//    QVertexTableModel* Model;
    int                oldNumPoints;
private slots:
    void on_oldradius_valueChanged(double arg1);
    void on_numPoints_valueChanged(int arg1);
    void on_bedradius_valueChanged(double arg1);
    void on_numfactors_currentIndexChanged(const QString &arg1);
    void on_pbStart_clicked();
    void on_copyButton_clicked();
    void on_pbRecalc_clicked();
    void on_pushButton_clicked();
    void on_cbAutoManual_clicked();
    void on_autoCheckBox_clicked();
    void on_homeAllButton_clicked();
    void slotBPCliked(int code);
    void slotValueChanged(int code);
    void slotNewPositionReady(const TVertex& ver);
    void on_clearAllValues_clicked();
private:
};

#endif // ESCHER3D_WND_H
