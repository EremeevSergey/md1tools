#include "opendact_user_var.h"

//bool CHeights::firstHeights = true;

CHeights::CHeights()
{
}

CHeights::CHeights(float _center, float _X, float _XOpp, float _Y, float _YOpp, float _Z, float _ZOpp)
{
    center = _center;
    X      = _X;
    XOpp   = _XOpp;
    Y      = _Y;
    YOpp   = _YOpp;
    Z      = _Z;
    ZOpp   = _ZOpp;

    if (firstHeights == true){
        teX    = _X;
        teXOpp = _XOpp;
        teY    = _Y;
        teYOpp = _YOpp;
        teZ    = _Z;
        teZOpp = _ZOpp;
        firstHeights = false;
    }
}


CUserVariables::CUserVariables()
{

}
