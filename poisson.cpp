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

    if(vitesse.length() != 0){
        qDebug() << "v=" << vitesse.length();
        modelMatrix.rotate(0,0,1,0);
        qDebug() << "y/v=" << acos(vitesse.y()/vitesse.length())*180.0 / M_PI;
        modelMatrix.rotate(acos(vitesse.y()/vitesse.length())*180.0 / M_PI - 90.0,0,0,1);
    }
    program_poisson->setUniformValue("modelMatrix", modelMatrix);
    program_poisson->setUniformValue("pSize", size);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}
