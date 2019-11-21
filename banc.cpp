#include "banc.h"

Banc::Banc(int nbPoissons, double largeur, double hauteur, double profondeur)
{
    this->nbPoissons=nbPoissons;
    this->largeurAquarium=largeur;
    this->hauteurAquarium=hauteur;
    this->profondeurAquarium=profondeur;
    initPoissons(QVector3D(0.0, 0.0, 0.0), 5.0,100);
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
    double nbvoisin,nbvoisinage;       //voisinage = voisin + obstacle + predateur
    double coefDist,coefVoisinage;
    QVector3D tmpSeparation, tmpAlignement, tmpCohesion, projMur;
    for(int i = 0; i < nbPoissons; i++){
        nbvoisin=0;
        tmpSeparation=QVector3D(0,0,0);
        tmpAlignement=QVector3D(0,0,0);
        tmpCohesion=QVector3D(0,0,0);
        for(int j = 0; j < nbPoissons; j++){
            if( i!=j &&poissons[i].dans_voisinage(poissons[j].position, distanceVoisinage)){
                coefDist=(distanceVoisinage-(poissons[i].position.distanceToPoint(poissons[j].position)));
                //coefDist=1/(poissons[i].position.distanceToPoint(poissons[j].position));
                nbvoisin+=coefDist;

                tmpSeparation+=(poissons[j].position-poissons[i].position)*coefDist;
                tmpAlignement+=(poissons[j].vitesse-poissons[i].vitesse)*coefDist;
                tmpCohesion+=(poissons[j].position-poissons[i].position)*coefDist;
            }
        }
        nbvoisinage=nbvoisin;

        // fear the box
        int signe1,signe2;
        coefVoisinage=1+nbvoisin/2.0;
        for(int k=0; k<1;k++){
            if( largeurAquarium/2    < poissons[i].position.x() + distanceVoisinage){
                coefDist=std::max(distanceVoisinage -(largeurAquarium/2-poissons[i].position.x() ),0.0);
                nbvoisinage+=coefDist;
                signe1=1;signe2=1;
                if(poissons[i].position.y()>0) signe1=-1;
                if(poissons[i].position.z()>0) signe2=-1;
                tmpSeparation+=QVector3D(1,0.1*signe1,0.1*signe2)*(coefDist)*(coefVoisinage);

            } if(-largeurAquarium/2    > poissons[i].position.x() - distanceVoisinage){
                coefDist=std::max(distanceVoisinage -(largeurAquarium/2+poissons[i].position.x() ),0.0);
                nbvoisinage+=coefDist;
                signe1=1;signe2=1;
                if(poissons[i].position.y()<0) signe1=-1;
                if(poissons[i].position.z()<0) signe2=-1;
                tmpSeparation+=QVector3D(-1,0.1*signe1,0.1*signe2)*(coefDist)*(coefVoisinage);

            }if( hauteurAquarium/2    < poissons[i].position.y() + distanceVoisinage){
                coefDist=std::max(distanceVoisinage -(hauteurAquarium/2-poissons[i].position.y() ),0.0);
                nbvoisinage+=coefDist;
                signe1=1;signe2=1;
                if(poissons[i].position.x()<0) signe1=-1;
                if(poissons[i].position.z()<0) signe2=-1;
                tmpSeparation+=QVector3D(0.1*signe1,1,0.1*signe2)*(coefDist)*(coefVoisinage);

            }if(-hauteurAquarium/2    > poissons[i].position.y() - distanceVoisinage){
                coefDist=std::max(distanceVoisinage -(hauteurAquarium/2+poissons[i].position.y() ),0.0);
                nbvoisinage+=coefDist;
                signe1=1;signe2=1;
                if(poissons[i].position.x()<0) signe1=-1;
                if(poissons[i].position.z()<0) signe2=-1;
                tmpSeparation+=QVector3D(0.1*signe1,-1,0.1*signe2)*(coefDist)*(coefVoisinage);

            }if( profondeurAquarium/2 < poissons[i].position.z() + distanceVoisinage){
                coefDist=std::max(distanceVoisinage -(profondeurAquarium/2-poissons[i].position.z() ),0.0);
                nbvoisinage+=coefDist;
                signe1=1;signe2=1;
                if(poissons[i].position.x()<0) signe1=-1;
                if(poissons[i].position.y()<0) signe2=-1;
                tmpSeparation+=QVector3D(0.1*signe1,0.1*signe2,1)*(coefDist)*(coefVoisinage);

            } if(-profondeurAquarium/2 > poissons[i].position.z() - distanceVoisinage){
                coefDist=std::max(distanceVoisinage -(profondeurAquarium/2+poissons[i].position.z() ),0.0);
                nbvoisinage+=coefDist;
                signe1=1;signe2=1;
                if(poissons[i].position.x()<0) signe1=-1;
                if(poissons[i].position.y()<0) signe2=-1;
                tmpSeparation+=QVector3D(0.1*signe1,0.1*signe2,-1)*(coefDist)*(coefVoisinage);
            }
        }

        //faire des truc pour separation

        if(nbvoisin!=0){
            tmpAlignement/=nbvoisin;
            tmpCohesion/=nbvoisin;

            tmpAlignement.normalize();
            tmpCohesion.normalize();
        }
        if(nbvoisinage !=0){

            tmpSeparation/=nbvoisinage;
            tmpSeparation=-(tmpSeparation);
            tmpSeparation.normalize();
        }

        vSeparation.push_back(tmpSeparation);
        vAlignement.push_back(tmpAlignement);
        vCohesion.push_back(tmpCohesion);
    }

    for(int i = 0; i < nbPoissons; i++){
        QVector3D vitessePrecedent=poissons[i].vitesse;
        vitessePrecedent.normalize();
        poissons[i].vitesse= vitessePrecedent * poidsPrecedent + vSeparation[i] * poidsSeparation + vAlignement[i] * poidsAlignement +  vCohesion[i] * poidsCohesion;

        if(poissons[i].vitesse.length()>1){
            poissons[i].vitesse.normalize();
        }
        poissons[i].vitesse*=vitessePoisson;






        poissons[i].animate(dt);
    }

}

void Banc::affiche(QOpenGLShaderProgram *program_poisson){
    for(int i = 0; i < nbPoissons; i++){
        poissons[i].affiche(program_poisson);
    }
}
