#ifndef OPENDACT_WND_H
#define OPENDACT_WND_H

#include <QWidget>
#include <QCheckBox>
#include "ui_opendact_wnd.h"
#include "../../gui/basewnd.h"
#include "core/printer/printer.h"
#include "heightmap_widget.h"
#include "opendact/opendact.h"
#include "heightmap_widget.h"
#include "2dchart.h"

class COpendactWnd : public CBaseWindow,public Ui::opendact_wnd
{
    Q_OBJECT
protected:
    void mainLoop      ();
    void activeLoop    ();
    void checkHeights  ();
    void checkZProbe   ();
    void calibrate     ();

public:
    explicit COpendactWnd(QWidget *parent = 0);
    ~COpendactWnd();
protected:
    int  iteration;
    int  currentPosition;
    bool checkHeightsOnly;
    bool readEepromOnly;
protected:
    void setButtonValues ();
    void setEEPROMGUIList();
    void setUserVariables();
    void setHeightsInvoke();
protected:
    CHeightmapWidget *FirstHeightMap;
    CHeightmapWidget *CurrentHeightMap;

    CHeightmapWidget *InputHeightMap;
    CHeightmapWidget *ExpectedHeightMap;

    C2dChart         *accuracyTime;
    QCheckBox        *autoCheckBox;
    void initUi();
    void updateControls();
signals:
    void signalNextStep();
private slots:
    void slotLogConsole      (const QString& str);
    void slotLogConsole      (const QString& str,Qt::GlobalColor color);
    void slotSetAccuracyPoint(int iteration_num, float temp_accuracy);
    void slotCommandExecuted ();
    void slotReady           (const QString&);
private slots:
    void on_openAdvanced_clicked();
    void on_calibrateButton_clicked();
    void on_stopBut_clicked();
    void on_checkHeightsBut_clicked();
    void on_readEepromMan_clicked();
    void on_manualCalibrateBut_clicked();
    void on_autoCheckBox(bool val);
    void onInputPushButtonClicked(CHeightmapWidget::EBushButtons but);
    void on_sendEepromMan_clicked();
    void slotNewPosition(const TVertex& ver);
};

#endif // OPENDACT_WND_H
