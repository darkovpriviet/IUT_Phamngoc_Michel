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
#define ROTATION 1
#define TRANSLATION 2

typedef struct _PidCorrector
{
double Kp;
double Ki;
double Kd;
double erreurProportionelleMax;
double erreurIntegraleMax;
double erreurDeriveeMax;
double erreurIntegrale;
double epsilon_1;
double erreur;
//For Debug only
double corrP;
double corrI;
double corrD;
}PidCorrector;

typedef struct _Ghost
{
 double ThetaGhost;
 double X;
 double Y;
 double ThetaWay;
 double ThetaRestant;
 double ThetaArret;
 double incrementTheta;
 double Distance;
 double DisPro;
 double HypoWay;
 double X_Ghost;
 double Y_Ghost;
 double X_Droite;
 double Y_Droite;
 double ProjeteY; 
 double Hypotenus;

 double ecartangle;
 double DisPar;
 double longRestant;

 
 

} Ghost;
extern Ghost Rotation;
extern Ghost longitunal;
extern void Longueur();
extern void RotationGhost();
extern void EtatGhost();
void Send_Ghost();
void Send_GhostLong();
extern double VitesseTheta;
extern PidCorrector PidX;
extern PidCorrector PidTheta;
extern float saveSpeed;
extern float saveSpeed_1;
extern int etapeghost;
double Projete( double xA, double yA,double bx, double by);
void Distance_to_waypoint();
void TransmitAsserv();
void UpdateAsservissement();
double Correcteur(volatile PidCorrector* PidCorr, double erreur);
void PWMSetSpeedConsignePolaire(float vitesseLineaire, float vitesseAngulaire);
void SetupPidAsservissement(volatile PidCorrector* PidCorr, double Kp, double Ki, double Kd, double proportionelleMax, double integralMax,double deriveeMax);
#define DISTROUES 0.218

#ifdef	__cplusplus
extern "C" {
#endif



#ifdef	__cplusplus
}
#endif

#endif	/* ASSERVISSEMENT_H */

