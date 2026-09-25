
#include <stdlib.h>

#include "asservissement.h"
#include "UART_Protocol.h"
#include "Robot.h"
#include "ToolBox.h"
#include "QEI.h"
#include "Utilities.h"
#include "math.h"
#include "Ghost.h"






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
    
//TransmitAsserv(robotState.PidX,0x69);
//TransmitAsserv(robotState.PidTheta,0x70);
}


void UpdateAsservissement1()
{
//.erreur = robotState.saveSpeed_Lineaire - robotState.vitesseLineaireFromOdometry;
robotState.PDAng.erreur = robotState.saveSpeed_Angulaire - robotState.vitesseAngulaireFromOdometry;
//robotState.CorrectionVitesseLineaire =Correcteur(&robotState.PidX, robotState.PidX.erreur);
robotState.PDAng.CorrectionVitesseAngulaire = Correcteur(&robotState.PDAng, robotState.PDAng.erreur);

PWMSetSpeedCommandPolaire(robotState.CorrectionVitesseLineaire,robotState.CorrectionVitesseAngulaire);
    
//TransmitAsserv(robotState.PidX,0x69);
//TransmitAsserv(robotState.PidTheta,0x70);
}



void TransmitAsserv(volatile PidCorrector* PidCorr, int code)
{
    unsigned char payload[44];
    
    getBytesFromFloat(payload, 0,  PidCorr->erreur);
    getBytesFromFloat(payload, 4,  PidCorr->vitesse);
    getBytesFromFloat(payload, 8,  PidCorr->Kp);
    getBytesFromFloat(payload, 12, PidCorr->corrP);
    getBytesFromFloat(payload, 16, PidCorr->erreurProportionelleMax);
    getBytesFromFloat(payload, 20, PidCorr->Ki);
    getBytesFromFloat(payload, 24, PidCorr->corrI);
    getBytesFromFloat(payload, 28, PidCorr->erreurIntegraleMax);
    getBytesFromFloat(payload, 32, PidCorr->Kd);
    getBytesFromFloat(payload, 36, PidCorr->corrD);
    getBytesFromFloat(payload, 40, PidCorr->erreurDeriveeMax);
    UartEncodeAndSendMessage(code,44,payload);

}




 

