
#include <stdlib.h>

#include "asservissement.h"
#include "UART_Protocol.h"
#include "Robot.h"
#include "ToolBox.h"
#include "QEI.h"
#include "Utilities.h"
#include "math.h"
#include "Ghost.h"
 extern volatile ROBOT_STATE_BITS robotState;
 extern volatile double VitesseTheta;
 static double angleOdoPrecedent = 0.0;




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


void UpdateAsservissementGhost() //kp=63 kd =16
{
 



//robotState.PDAng.erreur=ModuloByAngle(robotState.angleRadianFromOdometry,Rotation.ThetaGhost)-robotState.angleRadianFromOdometry;
    robotState.PDLin.erreur=longitunal.ThetaGhost;


//robotState.PDAng.Kp=robotState.PDAng.Kp;
robotState.CorrectionVitesseLineaire =Correcteur(&robotState.PDLin, robotState.PDLin.erreur);
robotState.CorrectionVitesseAngulaire = Correcteur(&robotState.PDAng, robotState.PDAng.erreur);

PWMSetSpeedCommandPolaire(robotState.CorrectionVitesseLineaire,robotState.CorrectionVitesseAngulaire);
    //kp 50/2
//TransmitAsserv();
}



void UpdateAngleUnwrap()
{
    double delta =
        robotState.angleRadianFromOdometry - angleOdoPrecedent;

    // Passage +PI -> -PI
    if (delta > M_PI)
        delta -= 2.0 * M_PI;

    // Passage -PI -> +PI
    if (delta < -M_PI)
        delta += 2.0 * M_PI;

    robotState.angleOdoCumule += delta;

    angleOdoPrecedent = robotState.angleRadianFromOdometry;
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




 

