#ifndef BANC_H
#define BANC_H

#include "poisson.h"

#include <vector>
#include <QVector3D>

class Banc
{
public:
    Banc();
    void initPoissons(QVector3D vitesse, float size);


    int nbPoissons = 0;
    std::vector<Poisson> vecPoisson;


protected:
    double rangePos = 1000.0;
    double rangeNeg = -1000.0;
    int coefDivision = 100;
};

#endif // BANC_H
