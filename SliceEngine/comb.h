/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#ifndef COMB_H
#define COMB_H

#include "utils/polygon.h"

class Comb
{
private:
    Polygons& boundery;

    int64_t* minX;
    int64_t* maxX;
    unsigned int* minIdx;
    unsigned int* maxIdx;

    PointMatrix matrix;
    Pointc sp;
    Pointc ep;

    bool preTest(Pointc startPoint, Pointc endPoint);    
    bool collisionTest(Pointc startPoint, Pointc endPoint);

    void calcMinMax();
    
    unsigned int getPolygonAbove(int64_t x);
    
    Pointc getBounderyPointWithOffset(unsigned int polygonNr, unsigned int idx);
    
public:
    Comb(Polygons& _boundery);
    ~Comb();
    
    bool checkInside(Pointc p);

    bool moveInside(Pointc* p, int distance = 100);
    
    bool calc(Pointc startPoint, Pointc endPoint, vector<Pointc>& combPoints);
};

#endif//COMB_H
