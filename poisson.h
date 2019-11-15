#ifndef POISSON_H
#define POISSON_H

#include <QVector3D>
#include <QOpenGLShaderProgram>

#include <QOpenGLBuffer>
#include <cmath>

class Poisson
{
public:
    Poisson();
    Poisson(QVector3D position, QVector3D vitesse, float size);

    void animate(float dt);
    bool dans_voisinage(QVector3D p,int distanceVoisinage);
    void affiche(QOpenGLShaderProgram *program_poisson);






    QVector3D position,vitesse;
    float size;
    //lifeTimeLeft,initialLifeTime;
    QMatrix4x4 modelMatrix;
};

#endif // POISSON_H
