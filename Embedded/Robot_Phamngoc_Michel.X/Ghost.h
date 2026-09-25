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
 double X_Ghost_depart;
 double Y_Ghost_depart;
 double ecart;

 
 

} Ghost;
extern Ghost Rotation;
extern Ghost longitunal;
extern void Longueur();
extern void RotationGhost();
extern void EtatGhost();
void Send_Ghost();
void Send_GhostLong();
double Projete( double xA, double yA,double bx, double by);
void Distance_to_waypoint();

#ifdef	__cplusplus
}
#endif

#endif	/* GHOST_H */

