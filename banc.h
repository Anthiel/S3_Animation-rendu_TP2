#ifndef BANC_H
#define BANC_H

#include "poisson.h"

#include <vector>
#include <QVector3D>

class Banc
{
public:
    Banc(int nbPoissons, double largeur=40, double hauteur=40, double profondeur=40);
    void initPoissons(QVector3D vitesse, float size, float coefPrecision);
    void anime(float dt);
    void affiche(QOpenGLShaderProgram *program_poisson);


    int nbPoissons = 0;
    float vitesseMax = 10;
    float poidsSeparation=1.5;
    float poidsAlignement=1;
    float poidsCohesion=1;
    int distanceVoisinage=5;
    std::vector<Poisson> poissons;

    double largeurAquarium;
    double hauteurAquarium;
    double profondeurAquarium;


};

#endif // BANC_H
