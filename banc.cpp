#include "banc.h"

Banc::Banc()
{

}

void Banc::initPoissons(){
    for(int i = 0; i < nbPoissons; i++){
        Poisson p;
        double posX = (rangeNeg + (rand() % static_cast<int>(rangePos - rangeNeg + 1)))/coefDivision;
        double posY = (rangeNeg + (rand() % static_cast<int>(rangePos - rangeNeg + 1)))/coefDivision;
        double posZ = (rangeNeg + (rand() % static_cast<int>(rangePos - rangeNeg + 1)))/coefDivision;

        QVector3D position(posX, posY, posZ);
        p.position = position;
        vecPoisson.push_back(p);
    }
}
