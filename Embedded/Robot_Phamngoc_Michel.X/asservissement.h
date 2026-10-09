/* 
 * File:   asservissement.h
 * Author: E306-PC2
 *
 * Created on 29 avril 2026, 18:29
 */

#ifndef ASSERVISSEMENT_H
#define	ASSERVISSEMENT_H
#define M_TO_PERCENT 32

#define VitesseThetaMax 10
#define AccelerationTheta 0.1
#define AccelerationLineaire 0.2
#define incrementAng 0.2
#define incrementLine 0.2
#define ATTENTE 0 
#define ROTATION 1
#define TRANSLATION 2

typedef struct _PidCorrector
{
float Kp;
float Ki;
float Kd;
float erreurProportionelleMax;
float erreurIntegraleMax;
float erreurDeriveeMax;
float erreurIntegrale;
float epsilon_1;
float erreur;
//For Debug only
float corrP;
float corrI;
float corrD;
float vitesse;
}PidCorrector;

extern PidCorrector PidX;
extern PidCorrector PidTheta;
extern PidCorrector PDAng;
extern PidCorrector PDLinear;



void TransmitAsserv();
void UpdateAsservissement();
void UpdateAsservissementGhost();
double Correcteur(volatile PidCorrector* PidCorr, double erreur);
void PWMSetSpeedConsignePolaire(float vitesseLineaire, float vitesseAngulaire);
void SetupPidAsservissement(volatile PidCorrector* PidCorr, double Kp, double Ki, double Kd, double proportionelleMax, double integralMax,double deriveeMax);
void UpdateAngleUnwrap();
#define DISTROUES 0.23
//0.218

#ifdef	__cplusplus
extern "C" {
#endif



#ifdef	__cplusplus
}
#endif

#endif	/* ASSERVISSEMENT_H */

