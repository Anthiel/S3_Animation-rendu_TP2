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
    void anime();


    int nbPoissons = 0;
    std::vector<Poisson> vecPoisson;

    double largeurAquarium = 10.0;
    double hauteurAquarium = 10.0;
    double profondeurAquarium = 10.0;


protected:
    float rangePos = 1000.0;
    float rangeNeg = -1000.0;
    int coefDivision = 100;
};

#endif // BANC_H
