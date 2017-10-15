#include "opendact_user_var.h"
#include "opendact.h"
#include <QtNumeric>
/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/
CHeights::CHeights()
{
//    firstHeights = true;
    position     = 0;
}

void CHeights::init()
{
    position = 0;
    X    = qQNaN();
    XOpp = qQNaN();
    Y    = qQNaN();
    YOpp = qQNaN();
    Z    = qQNaN();
    ZOpp = qQNaN();

}

int CHeights::setHeights(float probePosition)//,int position)
{
    float zMaxLength    = EEPROM.zMaxLength;
    float probingHeight = UserVariables.probingHeight;

    switch (position){
    case 0:
        probePosition = zMaxLength - probingHeight + probePosition;
        Heights.center = probePosition;
        position++;
        break;
    case 1:
        probePosition = Heights.center - (zMaxLength - probingHeight + probePosition);
        probePosition = -probePosition;
        Heights.X = probePosition;
        position++;
        break;
    case 2:
        probePosition = Heights.center - (zMaxLength - probingHeight + probePosition);
        probePosition = -probePosition;
        Heights.XOpp = probePosition;
        position++;
        break;
    case 3:
        probePosition = Heights.center - (zMaxLength - probingHeight + probePosition);
        probePosition = -probePosition;
        Heights.Y = probePosition;
        position++;
        break;
    case 4:
        probePosition = Heights.center - (zMaxLength - probingHeight + probePosition);
        probePosition = -probePosition;
        Heights.YOpp = probePosition;
        position++;
        break;
    case 5:
        probePosition = Heights.center - (zMaxLength - probingHeight + probePosition);
        probePosition = -probePosition;
        Heights.Z = probePosition;
        position++;
        break;
    case 6:
        probePosition = Heights.center - (zMaxLength - probingHeight + probePosition);
        probePosition = -probePosition;
        Heights.ZOpp = probePosition;
        position = 0;

        EEPROM.zMaxLength = Heights.center;

        break;
    }
    return position;
}

QString CHeights::toString()
{
    return  QString("Center:%1").arg(QString::number(center,'f',3))+
            QString    (" X:%1").arg(QString::number(X     ,'f',3))+
            QString (" XOpp:%1").arg(QString::number(XOpp  ,'f',3))+
            QString    (" Y:%1").arg(QString::number(Y     ,'f',3))+
            QString (" YOpp:%1").arg(QString::number(YOpp  ,'f',3))+
            QString    (" Z:%1").arg(QString::number(Z     ,'f',3))+
            QString (" ZOpp:%1").arg(QString::number(ZOpp  ,'f',3));
}
//        OpenDACT.heightsSet = true;

void CHeights::printHeights()
{
    OpenDACT.signalLogConsole(toString());
}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/
CUserVariables::CUserVariables()
{
    advancedCalibration = false;
    stepsCalcNumber = 0;
//    isInitiated = false;
    diagonalRodLength = qQNaN();
}
