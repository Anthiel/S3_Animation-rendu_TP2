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
    float vitessePoisson = 20;
    float poidsSeparation=1.5f/12;
    float poidsAlignement=1.0f/12;
    float poidsCohesion=1.3f/12;
    float poidsPrecedent=8.2f/12;
    int distanceVoisinage=6;
    std::vector<Poisson> poissons;

    double largeurAquarium;
    double hauteurAquarium;
    double profondeurAquarium;


};

#endif // BANC_H
