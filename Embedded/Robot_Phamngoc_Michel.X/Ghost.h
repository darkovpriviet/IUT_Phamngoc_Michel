/* 
 * File:   Ghost.h
 * Author: E304-PC3
 *
 * Created on 25 septembre 2026, 17:09
 */

#ifndef GHOST_H
#define	GHOST_H



#ifdef	__cplusplus
extern "C" {
#endif


typedef struct _Ghost
{
 float ThetaGhost;
 float X;
 float Y;
 float ThetaWay;
 float ThetaRestant;
 float ThetaArret;
 float incrementTheta;
 float Distance;
 float DisPro;
 float HypoWay;
 float X_Ghost;
 float Y_Ghost;
 float X_Droite;
 float Y_Droite;
 float ProjeteY; 
 float Hypotenus;

 float ecartangle;
 float DisPar;
 float longRestant;
 float X_Ghost_depart;
 float Y_Ghost_depart;
 float ecart;
 float Waysave;

 
 

} Ghost;
extern volatile Ghost Rotation;
extern Ghost longitunal;
void Longueur();
void RotationGhost();
void EtatGhost();
void Send_Ghost();
void Send_GhostLong();
double Projete( double xA, double yA,double bx, double by);
void Distance_to_waypoint();
void carre();



#ifdef	__cplusplus
}
#endif

#endif	/* GHOST_H */

