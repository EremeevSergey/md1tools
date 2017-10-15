#ifndef OPENDACT_USER_VAR_H
#define OPENDACT_USER_VAR_H

#include <QString>
#include <QList>

class CHeights
{
public:
    CHeights();
    int     setHeights(float probePosition);//,int position);
    void    init();
    QString toString();
    void    printHeights();
    int     position;
public:
    float center;
    float X;
    float XOpp;
    float Y;
    float YOpp;
    float Z;
    float ZOpp;
    float teX;
    float teXOpp;
    float teY;
    float teYOpp;
    float teZ;
    float teZOpp;
//    bool firstHeights;
};

class CUserVariables
{
public:
    CUserVariables();
    // public static class UserVariables
    float HRadRatio;
    float DRadRatio;
    float accuracy;
    float calculationAccuracy;
    float probingHeight;

    float offsetCorrection;
    float mainOppPerc;
    float towPerc;
    float oppPerc;

    float dRadCorrection;
    float dRadMainOppPerc;
    float dRadTowPerc;
    float dRadOppPerc;

    float alphaRotationPercentage;
    float deltaTower;
    float deltaOpp;
    float plateDiameter;
    float diagonalRodLength;
    float FSROffset;
    float probingSpeed;
    float xySpeed;//feedrate in gcode

    QString probeChoice;

    bool advancedCalibration;

    int pauseTimeSet;
    int advancedCalCount;
    int maxIterations;
    int l;

    QList<float> known_yDR;// = new List<float>();
    QList<float> known_xDR;// = new List<float>();

//    bool isInitiated;
    int stepsCalcNumber;

};

#endif // OPENDACT_USER_VAR_H
