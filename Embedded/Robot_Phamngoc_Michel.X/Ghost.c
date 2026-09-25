
#include <stdlib.h>

#include "asservissement.h"
#include "UART_Protocol.h"
#include "Robot.h"
#include "ToolBox.h"
#include "QEI.h"
#include "Utilities.h"
#include "math.h"

#include "Ghost.h"

Ghost Rotation;
Point P;
Point A;
Point B;
extern int etapeghost;
Ghost longitunal;
double VitesseTheta = 4;
double VitesseLineaire = 2;
void RotationGhost() {
    
Rotation.ThetaRestant= ModuloByAngle(Rotation.ThetaGhost,Rotation.ThetaWay)-Rotation.ThetaGhost;//]-pi, pi] 90 
            
    Rotation.ThetaArret = VitesseTheta*VitesseTheta /(2*AccelerationTheta);
            
    Rotation.incrementTheta =VitesseTheta/100 ;
    
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
       Rotation.ecart=0;
       

//       getBytesFromFloat(payload,48,Y);
 
       
   }

}



void Longueur() {

    
            
    longitunal.longRestant =  longitunal.HypoWay-longitunal.ThetaGhost;
    
    longitunal.ThetaArret = VitesseLineaire*VitesseLineaire /(2*AccelerationTheta);
            
    longitunal.incrementTheta =VitesseLineaire/100 ;
    
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
       longitunal.X_Ghost =   longitunal.X_Ghost_depart + longitunal.ThetaGhost * cos(Rotation.ThetaGhost);
       longitunal.Y_Ghost = longitunal.Y_Ghost_depart + longitunal.ThetaGhost * sin(Rotation.ThetaGhost);
   
    
//   UartEncodeAndSendMessage(0x81,72,payload);
    Send_GhostLong();
  
    
  
    
    
   if(VitesseLineaire==0 && Abs(longitunal.longRestant) <0.01){
       longitunal.ThetaGhost = longitunal.HypoWay;
     
       
       longitunal.X_Ghost_depart= longitunal.X_Ghost;
       longitunal.Y_Ghost_depart = longitunal.Y_Ghost;
      
 
         etapeghost=ATTENTE;
     
    
       
       
       
  
  }

}
void Waypoint(){
    float x,y;
    x= longitunal.X_Ghost_depart +longitunal.HypoWay*cos(Rotation.ecart);
    y= longitunal.Y_Ghost_depart +longitunal.HypoWay*sin(Rotation.ecart);
    
    unsigned char payload [8];
    getBytesFromFloat(payload,0,x);
    getBytesFromFloat(payload,4,y);
    UartEncodeAndSendMessage(0x83,8,payload);
}

void Send_Ghost(){
    
   unsigned char payload[24];
   getBytesFromFloat(payload,0,Rotation.X_Ghost);
   getBytesFromFloat(payload,4,Rotation.Y_Ghost);
   getBytesFromFloat(payload, 8,Rotation.ThetaGhost *180/M_PI);
   getBytesFromFloat(payload,12,Rotation.HypoWay);
   getBytesFromFloat(payload,16, longitunal.ThetaGhost);
   getBytesFromFloat(payload, 20,Rotation.ThetaGhost );
   
  
   
   UartEncodeAndSendMessage(0x81,24,payload);
    
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
        
        case ATTENTE: 
             longitunal.ThetaGhost =0;
             break;
        case ROTATION:
           RotationGhost();
            
            break; 
        
        case TRANSLATION:
            Longueur();
            break; 
        
        
    }
    
}



void Distance_to_waypoint(){
 
    //Rotation.DisPro= Rotation.HypoWay*cos(robotState.angleRadianFromOdometry-Rotation.ThetaWay);
    //Rotation.DisPar = sqrt(robotState.xPosFromOdometry*robotState.xPosFromOdometry+robotState.yPosFromOdometry*robotState.yPosFromOdometry);
    //Rotation.ecartangle = atan((Rotation.HypoWay*sin(Rotation.ThetaWay-robotState.angleRadianFromOdometry))/(Rotation.DisPro-Rotation.DisPar));
    P.x=Rotation.X;
    P.y=Rotation.Y;
    
    Rotation.DisPro = Projete(0,0,B.x,B.y);
}




double Projete( double xA, double yA,double bx, double by){
double dx = bx - xA;;
double dy = by - yA;
    
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
