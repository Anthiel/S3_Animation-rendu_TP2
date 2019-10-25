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

void Poisson::affiche(QOpenGLShaderProgram *program_poisson){
    modelMatrix.setToIdentity();
    modelMatrix.translate(position);

    if(vitesse.x() != 0 || vitesse.z() != 0){
        int signeAngle=1;
        if(vitesse.z() > 0) signeAngle=-1;
        float angleXZ=signeAngle *acos(vitesse.x()/sqrt(pow(vitesse.x(),2)+pow(vitesse.z(),2)))*180.0 / M_PI +180;
        qDebug() << "angleXZ" << angleXZ;
        modelMatrix.rotate(angleXZ,0,1,0);
    }
    if(vitesse.length() != 0){
        float angleY=acos(vitesse.y()/vitesse.length())*180.0 / M_PI - 90.0;
        qDebug() << "angleY" << angleY;
        modelMatrix.rotate(angleY,0,0,1);
    }
    program_poisson->setUniformValue("modelMatrix", modelMatrix);
    program_poisson->setUniformValue("pSize", size);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}
