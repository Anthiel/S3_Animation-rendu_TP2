#include "banc.h"

Banc::Banc()
{
    initPoissons(QVector3D(0.0, 0.0, 0.0), 3.0);
}

void Banc::initPoissons(QVector3D vitesse, float size){
    for(int i = 0; i < nbPoissons; i++){
        float posX = (rangeNeg + (rand() % static_cast<int>(rangePos - rangeNeg + 1)))/coefDivision;
        float posY = (rangeNeg + (rand() % static_cast<int>(rangePos - rangeNeg + 1)))/coefDivision;
        float posZ = (rangeNeg + (rand() % static_cast<int>(rangePos - rangeNeg + 1)))/coefDivision;

        QVector3D position(posX, posY, posZ);
        Poisson p(position, vitesse, size);
        vecPoisson.push_back(p);
    }
}

void Banc::anime(){

}
