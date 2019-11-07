#include "banc.h"

Banc::Banc(int nbPoissons, double largeur, double hauteur, double profondeur)
{
    this->nbPoissons=nbPoissons;
    this->largeurAquarium=largeur;
    this->hauteurAquarium=hauteur;
    this->profondeurAquarium=profondeur;
    initPoissons(QVector3D(0.0, 0.0, 0.0), 3.0,100);
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
    float distanceVoisinage,coefDist;
    QVector3D tmpSeparation, tmpAlignement, tmpCohesion, projMur;
    for(int i = 0; i < nbPoissons; i++){
        nbvoisin=0;
        tmpSeparation=QVector3D(0,0,0);
        tmpAlignement=QVector3D(0,0,0);
        tmpCohesion=QVector3D(0,0,0);
        for(int j = 0; j < nbPoissons; j++){
            if(poissons[i].dans_voisinage(poissons[j].position)){
                coefDist=poissons[i].position.distanceToPoint(poissons[j].position)*0.5;
                nbvoisin+=1/coefDist;
                tmpSeparation+=poissons[j].position/coefDist;
                tmpAlignement+=poissons[j].vitesse/coefDist;
                tmpCohesion+=poissons[j].position/coefDist;
            }
        }
        nbvoisinage=nbvoisin;
        distanceVoisinage=3;

        // fear the box
        for(int k=0; k<10;k++){
            if( largeurAquarium/2    < poissons[i].position.x() + distanceVoisinage){
                projMur=poissons[i].position;
                projMur.setX(largeurAquarium/2-1);
                coefDist=poissons[i].position.distanceToPoint(projMur);
                tmpSeparation+=projMur/coefDist;
                nbvoisinage+=1/coefDist;
            }
            if(-largeurAquarium/2    > poissons[i].position.x() - distanceVoisinage){
                projMur=poissons[i].position;
                projMur.setX(-largeurAquarium/2+1);
                coefDist=poissons[i].position.distanceToPoint(projMur);
                tmpSeparation+=projMur/coefDist;
                nbvoisinage+=1/coefDist;
            }
            if( hauteurAquarium/2    < poissons[i].position.y() + distanceVoisinage){
                projMur=poissons[i].position;
                projMur.setY(hauteurAquarium/2-1);
                coefDist=poissons[i].position.distanceToPoint(projMur);
                tmpSeparation+=projMur/coefDist;
                nbvoisinage+=1/coefDist;
            }
            if(-hauteurAquarium/2    > poissons[i].position.y() - distanceVoisinage){
                projMur=poissons[i].position;
                projMur.setY(-hauteurAquarium/2+1);
                coefDist=poissons[i].position.distanceToPoint(projMur);
                tmpSeparation+=projMur/coefDist;
                nbvoisinage+=1/coefDist;
            }
            if( profondeurAquarium/2 < poissons[i].position.z() + distanceVoisinage){
                projMur=poissons[i].position;
                projMur.setZ(profondeurAquarium/2-1);
                coefDist=poissons[i].position.distanceToPoint(projMur);
                tmpSeparation+=projMur/coefDist;
                nbvoisinage+=1/coefDist;
            }
            if(-profondeurAquarium/2 > poissons[i].position.z() - distanceVoisinage){
                projMur=poissons[i].position;
                projMur.setZ(-profondeurAquarium/2+1);
                coefDist=poissons[i].position.distanceToPoint(projMur);
                tmpSeparation+=projMur/coefDist;
                nbvoisinage+=1/coefDist;
            }
        }

        //faire des truc pour separation


        tmpSeparation/=nbvoisinage;
        tmpAlignement/=nbvoisin;
        tmpCohesion/=nbvoisin;

        vSeparation.push_back(-(tmpSeparation-poissons[i].position));
        //vAlignement.push_back(tmpAlignement-poissons[i].vitesse);
        vAlignement.push_back(poissons[i].vitesse-tmpAlignement);
        vCohesion.push_back(tmpCohesion-poissons[i].position);
    }

    for(int i = 0; i < nbPoissons; i++){
        poissons[i].vitesse= poissons[i].vitesse + vSeparation[i] * poidsSeparation + vAlignement[i] * poidsAlignement + vCohesion[i] * poidsCohesion;
        if(poissons[i].vitesse.length()>vitesseMax){
            poissons[i].vitesse.normalize();
            poissons[i].vitesse*=vitesseMax;
        }

        poissons[i].animate(dt);
    }

}

void Banc::affiche(QOpenGLShaderProgram *program_poisson){
    for(int i = 0; i < nbPoissons; i++){
        poissons[i].affiche(program_poisson);
    }
}
