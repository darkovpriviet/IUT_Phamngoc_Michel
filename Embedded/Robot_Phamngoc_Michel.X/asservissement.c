
#include <stdlib.h>

#include "asservissement.h"
#include "UART_Protocol.h"
#include "Robot.h"
#include "ToolBox.h"
#include "QEI.h"
#include "Utilities.h"
#include "math.h"

Ghost Rotation;
Point P;
Point A;
Point B;

Ghost longitunal;
double VitesseTheta = 4;
double VitesseLineaire = 2;




void SetupPidAsservissement(volatile PidCorrector* PidCorr, double Kp, double Ki, double Kd, double proportionelleMax, double integralMax,double deriveeMax){
PidCorr->Kp = Kp;
PidCorr->erreurProportionelleMax = proportionelleMax; //On limite la correction due au Kp
PidCorr->Ki = Ki;
PidCorr->erreurIntegraleMax = integralMax; //On limite la correction due au Ki
PidCorr->Kd = Kd;
PidCorr->erreurDeriveeMax = deriveeMax;


}
void PWMSetSpeedConsignePolaire(float vitesseLineaire, float vitesseAngulaire) {
    robotState.saveSpeed_Lineaire=vitesseLineaire;
robotState.saveSpeed_Angulaire=vitesseAngulaire;
}



//
void PWMSetSpeedCommandPolaire(float vitesseLineaire, float vitesseAngulaire) {
    /*
     
     
     PWMSetSpeedConsignePolaire(0,0) ,n'arrete pas le moteur a régler
     
     */
robotState.vitesseDroiteConsigne = vitesseLineaire + vitesseAngulaire*(0.218/2);   //si consigne=20m/s vitesse=0.5m/s facteur de 40 entre les deux 
robotState.vitesseGaucheConsigne = vitesseLineaire - vitesseAngulaire*(0.218/2);   
robotState.vitesseDroitePercent = -M_TO_PERCENT * robotState.vitesseDroiteConsigne;
robotState.vitesseGauchePercent = M_TO_PERCENT * robotState.vitesseGaucheConsigne;
LimitToInterval(robotState.vitesseDroitePercent , -100, 100);
LimitToInterval(robotState.vitesseGauchePercent , -100, 100);
}
 //kp quand oscille mettre Kp/2 ki 30 pourcent quand il oscille  kd =0,5 pas a mettre pour l instant 
double Correcteur(volatile PidCorrector* PidCorr, double erreur)
{
PidCorr->erreur = erreur;
double erreurProportionnelle = LimitToInterval(erreur,-PidCorr->erreurProportionelleMax/PidCorr->Kp,PidCorr->erreurProportionelleMax/PidCorr->Kp); 
PidCorr->corrP = PidCorr->Kp*erreurProportionnelle;
PidCorr->erreurIntegrale = PidCorr->erreurIntegrale + erreur/FREQ_ECH_QEI ;

PidCorr->erreurIntegrale = LimitToInterval(PidCorr->erreurIntegrale,- PidCorr->erreurIntegraleMax/PidCorr->Ki, PidCorr->erreurIntegraleMax/PidCorr->Ki);
PidCorr->corrI = PidCorr->Ki* PidCorr->erreurIntegrale;
double erreurDerivee = (erreur - PidCorr->epsilon_1)*FREQ_ECH_QEI;
double deriveeBornee = LimitToInterval(erreurDerivee, -PidCorr->erreurDeriveeMax/PidCorr->Kd,
PidCorr->erreurDeriveeMax/PidCorr->Kd);
PidCorr->epsilon_1 = erreur;
PidCorr->corrD = deriveeBornee * PidCorr->Kd;
return PidCorr->corrP+PidCorr->corrI+PidCorr->corrD;
}

void UpdateAsservissement()
{
robotState.PidX.erreur = robotState.saveSpeed_Lineaire - robotState.vitesseLineaireFromOdometry;
robotState.PidTheta.erreur = robotState.saveSpeed_Angulaire - robotState.vitesseAngulaireFromOdometry;
robotState.CorrectionVitesseLineaire =Correcteur(&robotState.PidX, robotState.PidX.erreur);
robotState.CorrectionVitesseAngulaire = Correcteur(&robotState.PidTheta, robotState.PidTheta.erreur);

PWMSetSpeedCommandPolaire(robotState.CorrectionVitesseLineaire,robotState.CorrectionVitesseAngulaire);
    
TransmitAsserv();
}

void TransmitAsserv()
{
    unsigned char payload[88];
    
    getBytesFromFloat(payload, 0,  robotState.PidX.erreur);
    getBytesFromFloat(payload, 4,  robotState.vitesseLineaireFromOdometry);
    getBytesFromFloat(payload, 8,  robotState.PidX.Kp);
    getBytesFromFloat(payload, 12, robotState.PidX.corrP);
    getBytesFromFloat(payload, 16, robotState.PidX.erreurProportionelleMax);
    getBytesFromFloat(payload, 20, robotState.PidX.Ki);
    getBytesFromFloat(payload, 24, robotState.PidX.corrI);
    getBytesFromFloat(payload, 28, robotState.PidX.erreurIntegraleMax);
    getBytesFromFloat(payload, 32, robotState.PidX.Kd);
    getBytesFromFloat(payload, 36, robotState.PidX.corrD);
    getBytesFromFloat(payload, 40, robotState.PidX.erreurDeriveeMax);

    getBytesFromFloat(payload, 44, robotState.PidTheta.erreur);
    getBytesFromFloat(payload, 48, robotState.vitesseAngulaireFromOdometry);
    getBytesFromFloat(payload, 52, robotState.PidTheta.Kp);
    getBytesFromFloat(payload, 56, robotState.PidTheta.corrP);
    getBytesFromFloat(payload, 60, robotState.PidTheta.erreurProportionelleMax);
    getBytesFromFloat(payload, 64, robotState.PidTheta.Ki);
    getBytesFromFloat(payload, 68, robotState.PidTheta.corrI);
    getBytesFromFloat(payload, 72, robotState.PidTheta.erreurIntegraleMax);
    getBytesFromFloat(payload, 76, robotState.PidTheta.Kd);
    getBytesFromFloat(payload, 80, robotState.PidTheta.corrD);
    getBytesFromFloat(payload, 84, robotState.PidTheta.erreurDeriveeMax); 
    UartEncodeAndSendMessage(0x69,88,payload);

}




 





void RotationGhost() {
    
Rotation.ThetaRestant= ModuloByAngle(Rotation.ThetaGhost,Rotation.ThetaWay)-Rotation.ThetaGhost;//]-pi, pi] 90 
            
    Rotation.ThetaArret = VitesseTheta*VitesseTheta /(2*AccelerationTheta);
            
    Rotation.incrementTheta =VitesseTheta/FREQ_ECH_QEI ;
    
        if(VitesseTheta<0){
        Rotation.ThetaArret=-Rotation.ThetaArret;
    }
    
    if(((Rotation.ThetaArret >= 0 && Rotation.ThetaRestant>=0) || (Rotation.ThetaArret <= 0 && Rotation.ThetaRestant <=0)) && (Abs(Rotation.ThetaRestant) >= Abs(Rotation.ThetaArret)))
    {
        if (Rotation.ThetaRestant > 0) {
            VitesseTheta = Min(VitesseTheta + AccelerationTheta/ FREQ_ECH_QEI,VitesseThetaMax);
        }
        else if (Rotation.ThetaRestant < 0) {
            VitesseTheta = Max(VitesseTheta - AccelerationTheta/ FREQ_ECH_QEI,-VitesseThetaMax);// 
        }        
    }
    
    else {
        
        if (VitesseTheta >0) {
            
           VitesseTheta = Min(VitesseTheta - AccelerationTheta/ FREQ_ECH_QEI,0);
        }
        
        else if (VitesseTheta <0) {
           VitesseTheta = Max(VitesseTheta + AccelerationTheta/ FREQ_ECH_QEI,0);
        }
        if (Abs(Rotation.ThetaRestant) < Abs(incrementAng)){
            Rotation.incrementTheta = Rotation.ThetaRestant;
        }
    }
   
   Rotation.ThetaGhost = Rotation.ThetaGhost + Rotation.incrementTheta;
  
//   UartEncodeAndSendMessage(0x81,72,payload);
 
   if(VitesseTheta==0 && Abs(Rotation.ThetaRestant) <0.01){
       Rotation.ThetaGhost = Rotation.ThetaWay;
       Rotation.X_Ghost=Rotation.X;
       Rotation.Y_Ghost=Rotation.Y;
        etapeghost=TRANSLATION;

//       getBytesFromFloat(payload,48,Y);
 
       
   }

}



void Longueur() {

    
            
    longitunal.longRestant =  Rotation.HypoWay-longitunal.ThetaGhost;
    
    longitunal.ThetaArret = VitesseLineaire*VitesseLineaire /(2*AccelerationTheta);
            
    longitunal.incrementTheta =VitesseLineaire/FREQ_ECH_QEI ;
    
        if(VitesseLineaire<0){
        longitunal.ThetaArret=-longitunal.ThetaArret;
    }
    
    if(((longitunal.ThetaArret >= 0 && longitunal.longRestant>=0) || (longitunal.ThetaArret <= 0 && longitunal.longRestant <=0)) && (Abs(longitunal.longRestant) >= Abs(longitunal.ThetaArret)))
    {
        if (longitunal.longRestant > 0) {
            VitesseLineaire = Min(VitesseLineaire + AccelerationLineaire/ FREQ_ECH_QEI,VitesseThetaMax);
        }
        else if (longitunal.longRestant < 0) {
            VitesseLineaire = Max(VitesseLineaire - AccelerationLineaire/ FREQ_ECH_QEI,-VitesseThetaMax);// 
        }        
    }
    
    else {
        
        if (VitesseLineaire >0) {
            
           VitesseLineaire = Min(VitesseLineaire - AccelerationLineaire/ FREQ_ECH_QEI,0);
        }
        
        else if (VitesseLineaire <0) {
           VitesseLineaire = Max(VitesseLineaire + AccelerationLineaire/ FREQ_ECH_QEI,0);
        }
        if (Abs(longitunal.longRestant) < Abs(incrementLine)){
            longitunal.incrementTheta = longitunal.longRestant;
        }
    }
   
   longitunal.ThetaGhost = longitunal.ThetaGhost + longitunal.incrementTheta;
    longitunal.X_Ghost = longitunal.ThetaGhost * cos(Rotation.ThetaGhost);
    longitunal.Y_Ghost = longitunal.ThetaGhost * sin(Rotation.ThetaGhost);
    
//   UartEncodeAndSendMessage(0x81,72,payload);
    Send_GhostLong();
  
    
  
    
    
   if(VitesseLineaire==0 && Abs(longitunal.longRestant) <0.01){
       longitunal.ThetaGhost = Rotation.DisPro;
     
    
       
       
       
  
  }

}
void Send_Ghost(){
    
   unsigned char payload[20];
   getBytesFromFloat(payload,0,Rotation.X_Ghost);
   getBytesFromFloat(payload,4,Rotation.Y_Ghost);
   getBytesFromFloat(payload, 8,Rotation.ThetaGhost *180/M_PI);
   getBytesFromFloat(payload,12,Rotation.HypoWay);
   getBytesFromFloat(payload,16, longitunal.ThetaGhost);
   
  
   
   UartEncodeAndSendMessage(0x81,20,payload);
    
}
void Send_GhostLong(){
    double view=distancePointDroite(P,A,B);
     unsigned char payload[24];
     getBytesFromFloat(payload,0,longitunal.X_Ghost);
     getBytesFromFloat(payload,4,longitunal.Y_Ghost);
     getBytesFromFloat(payload,8, Rotation.DisPro);
     getBytesFromFloat(payload,12,B.x);
     getBytesFromFloat(payload,16,B.y);
     
     
     UartEncodeAndSendMessage(0x82,24,payload);
}

void EtatGhost(){
    
    
    switch(etapeghost){
        
        case ROTATION:
            
            RotationGhost();
            
            break; 
        
        case TRANSLATION:
            Longueur();
            break; 
        
        
    }
    
}



void Distance_to_waypoint(){
    Rotation.HypoWay=sqrt((Rotation.X*Rotation.X)+(Rotation.Y*Rotation.Y));
    //Rotation.DisPro= Rotation.HypoWay*cos(robotState.angleRadianFromOdometry-Rotation.ThetaWay);
    //Rotation.DisPar = sqrt(robotState.xPosFromOdometry*robotState.xPosFromOdometry+robotState.yPosFromOdometry*robotState.yPosFromOdometry);
    //Rotation.ecartangle = atan((Rotation.HypoWay*sin(Rotation.ThetaWay-robotState.angleRadianFromOdometry))/(Rotation.DisPro-Rotation.DisPar));
    P.x=Rotation.X;
    P.y=Rotation.Y;
    
    Rotation.DisPro = Projete(0,0,B.x,B.y);
    
 
}



double Projete( double xA, double yA,double bx, double by){
double dx = b.x - xA;;
double dy = b.y - yA;
    
double t =
    (( Rotation.X - xA) * dx + ( Rotation.Y - yA) * dy)
    / (dx * dx + dy * dy);

double xp = xA + t * dx;
double yp = yA + t * dy;

double distance =
    sqrt(
        (xp - xA) * (xp - xA) +
        (yp - yA) * (yp - yA)
    );

return distance;
}