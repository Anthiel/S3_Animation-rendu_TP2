#include "poisson.h"

Poisson::Poisson()
{
    this->position={0,0,0};
    this->vitesse={0,0,0};
    this->size=1;
}


Poisson::Poisson(QVector3D position, QVector3D vitesse, float size)
{
    this->position=position;
    this->vitesse=vitesse;
    this->size=size;
}

bool Poisson::dans_voisinage( QVector3D p)
{
    return distance<position.distanceToPoint(p);
}

void Poisson::animate(float dt)
{
    this->position+=dt*vitesse;
}
