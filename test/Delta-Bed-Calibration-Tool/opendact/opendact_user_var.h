#ifndef USER_VAR_H
#define USER_VAR_H

#include <QString>
#include <QList>

class CHeights
{
    //store every set of heights
public:
    CHeights();
    CHeights(float _center, float _X, float _XOpp, float _Y, float _YOpp, float _Z, float _ZOpp);
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
//    static bool firstHeights;
};

class CUserVariables
{
public:
    CUserVariables();
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

    bool advancedCalibration = false;

    int pauseTimeSet;
    int advancedCalCount;
    int maxIterations;
    int l;

    QList<float> known_yDR;
    QList<float> known_xDR;

    bool isInitiated = false;
    int stepsCalcNumber = 0;
};

#endif // USER_VAR_H
