#include "banc.h"

Banc::Banc(int nbPoissons, double largeur, double hauteur, double profondeur)
{
    this->nbPoissons=nbPoissons;
    this->largeurAquarium=largeur;
    this->hauteurAquarium=hauteur;
    this->profondeurAquarium=profondeur;
    initPoissons(QVector3D(0.0, 0.0, 0.0), 2.0,100);
}

void Banc::initPoissons(QVector3D vitesse, float size, float coefPrecision){
    poissons.clear();
    for(int i = 0; i < nbPoissons; i++){
        float posX = -largeurAquarium/4.0f +    (rand() % static_cast<int>((largeurAquarium/2)    *coefPrecision + 1))/coefPrecision;
        float posY = -hauteurAquarium/4.0f +    (rand() % static_cast<int>((hauteurAquarium/2)    *coefPrecision + 1))/coefPrecision;
        float posZ = -profondeurAquarium/4.0f + (rand() % static_cast<int>((profondeurAquarium/2) *coefPrecision + 1))/coefPrecision;

        QVector3D position(posX, posY, posZ);
        Poisson p(position, vitesse, size);
        poissons.push_back(p);
    }
    poissons[0].vitesse=QVector3D(1,0,0);
}

void Banc::anime(float dt){
    std::vector<QVector3D> vSeparation, vAlignement, vCohesion;
    float nbvoisin,nbvoisinage;       //voisinage = voisin + obstacle + predateur
    float coefDist;
    QVector3D tmpSeparation, tmpAlignement, tmpCohesion, projMur;
    for(int i = 0; i < nbPoissons; i++){
        nbvoisin=0;
        tmpSeparation=QVector3D(0,0,0);
        tmpAlignement=QVector3D(0,0,0);
        tmpCohesion=QVector3D(0,0,0);
        for(int j = 0; j < nbPoissons; j++){
            if( i!=j &&poissons[i].dans_voisinage(poissons[j].position, distanceVoisinage)){
                coefDist=poissons[i].position.distanceToPoint(poissons[j].position);
                nbvoisin+=1/coefDist;
                tmpSeparation+=(poissons[j].position-poissons[i].position)/coefDist;
                tmpAlignement+=(poissons[j].vitesse-poissons[i].vitesse)/coefDist;
                tmpCohesion+=(poissons[j].position-poissons[i].position)/coefDist;
            }
        }
        nbvoisinage=nbvoisin;

        // fear the box

        for(int k=0; k<1;k++){
            if( largeurAquarium/2    < poissons[i].position.x() + distanceVoisinage){
                coefDist=abs(largeurAquarium/2-poissons[i].position.x());
                nbvoisinage+=1/coefDist;
                tmpSeparation+=QVector3D(20,0,0);
            }
            else if(-largeurAquarium/2    > poissons[i].position.x() - distanceVoisinage){
                coefDist=abs(-largeurAquarium/2+poissons[i].position.x());
                nbvoisinage+=1/coefDist;
                tmpSeparation+=QVector3D(-20,0,0);
            }
            if( hauteurAquarium/2    < poissons[i].position.y() + distanceVoisinage){
                coefDist=abs(hauteurAquarium/2-poissons[i].position.y());
                nbvoisinage+=1/coefDist;
                tmpSeparation+=QVector3D(0,20,0);
            }
            else if(-hauteurAquarium/2    > poissons[i].position.y() - distanceVoisinage){
                coefDist=abs(-hauteurAquarium/2+poissons[i].position.y());
                nbvoisinage+=1/coefDist;
                tmpSeparation+=QVector3D(0,-20,0);
            }
            if( profondeurAquarium/2 < poissons[i].position.z() + distanceVoisinage){
                coefDist=abs(profondeurAquarium/2-poissons[i].position.z());
                nbvoisinage+=1/coefDist;
                tmpSeparation+=QVector3D(0,0,20);
            }
            else if(-profondeurAquarium/2 > poissons[i].position.z() - distanceVoisinage){
                coefDist=abs(-profondeurAquarium/2+poissons[i].position.z());
                nbvoisinage+=1/coefDist;
                tmpSeparation+=QVector3D(0,0,-20);
            }
        }

        //faire des truc pour separation

        if(nbvoisin!=0){
            tmpSeparation/=nbvoisinage;
            tmpAlignement/=nbvoisin;
            tmpCohesion/=nbvoisin;


            tmpSeparation=-(tmpSeparation);


            tmpSeparation.normalize();
            tmpAlignement.normalize();
            tmpCohesion.normalize();
        }

        vSeparation.push_back(tmpSeparation);
        vAlignement.push_back(tmpAlignement);
        vCohesion.push_back(tmpCohesion);
    }

    for(int i = 0; i < nbPoissons; i++){
        poissons[i].vitesse= poissons[i].vitesse + vSeparation[i] * poidsSeparation + vAlignement[i] * poidsAlignement +  vCohesion[i] * poidsCohesion;
        poissons[i].vitesse.normalize();
        poissons[i].vitesse*=vitesseMax;
        poissons[i].animate(dt);
    }

}

void Banc::affiche(QOpenGLShaderProgram *program_poisson){
    for(int i = 0; i < nbPoissons; i++){
        poissons[i].affiche(program_poisson);
    }
}
