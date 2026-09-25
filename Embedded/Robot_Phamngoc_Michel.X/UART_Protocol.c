#include <xc.h>
#include <math.h>
#include "UART_Protocol.h"
#include "IO.h"
#include "PWM.h"
#include "CB_TX1.h"
#include "main.h"
#include "asservissement.h"
#include "ToolBox.h"
#include "Robot.h"
#include "Utilities.h"
#include "Ghost.h"
int msgDecodedFunction = 0;
int msgDecodedPayloadLength = 0;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex = 0;
int receivedChecksum, calculatedChecksum = 0x00;
int rcvState = 0;
int autoControlActivated=0;
PidCorrector PidX;
PidCorrector PDAng;
int etapeghost;
float a;
float b;
float c;



unsigned char UartCalculateChecksum(int msgFunction,
        int msgPayloadLength, unsigned char* msgPayload) {
    //Fonction prenant entree la trame et sa longueur pour calculer le checksum
    unsigned char checksum = 0x00;

    checksum ^= 0xFE;

    checksum ^= msgFunction >> 8;
    checksum ^= msgFunction;

    checksum ^= msgPayloadLength >> 8;
    checksum ^= msgPayloadLength;

    for (int i = 0; i < msgPayloadLength; i++) {
        checksum ^= msgPayload[i];
    }
    return checksum;

}

static unsigned char trame[1024];

void UartEncodeAndSendMessage(int msgFunction,
        int msgPayloadLength, unsigned char* msgPayload) {
    //Fonction d?encodage et d?envoi d?un message
    //unsigned char trame[msgPayloadLength + 6];
    int pos = 0;
    trame[pos++] = 0xFE;
    trame[pos++] = msgFunction >> 8;
    trame[pos++] = msgFunction >> 0;
    trame[pos++] = msgPayloadLength >> 8;
    trame[pos++] = msgPayloadLength >> 0;

    for (int i = 0; i < msgPayloadLength; i++)
        trame[pos++] = (unsigned) (msgPayload[i]);

    trame[pos++] = UartCalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
    SendMessage(trame, pos);
}

void UartDecodeMessage(unsigned char c) {

    //Fonction prenant en entree un octet et servant a reconstituer les trames

    switch (rcvState) {
        case Waiting:
            if (c == 0xFE)
                rcvState = FunctionMSB;
            break;

        case FunctionMSB:
            msgDecodedFunction = c;
            rcvState = FunctionLSB;

            break;
        case FunctionLSB:
            msgDecodedFunction += c;
            rcvState = PayloadLengthMSB;
            break;
        case PayloadLengthMSB:
            msgDecodedPayloadLength = (c << 8);
            rcvState = PayloadLengthLSB;

            break;
        case PayloadLengthLSB:
            msgDecodedPayloadLength += c;
            rcvState = Payload;
            break;
        case Payload:

            msgDecodedPayload[msgDecodedPayloadIndex] = c;
            msgDecodedPayloadIndex++;
            if (msgDecodedPayloadIndex >= msgDecodedPayloadLength) {
                rcvState = CheckSum;
                msgDecodedPayloadIndex = 0;
            }
            break;
        case CheckSum:

            calculatedChecksum = UartCalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);

            if (calculatedChecksum == c) {
                UartProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            }
            rcvState = Waiting;
            break;
        default:
            rcvState = Waiting;
            break;
    }

}

 
void UartProcessDecodedMessage(int function,
        int payloadLength, unsigned char* payload) {
    //Fonction appelee apres le decodage pour executer l?action
    //correspondant au message recu
    
    switch (function) {
        
        case 0x0020:
            if (payload[0] == '1') {
                if (payload[1] == '1')
                    LED_BLANCHE_1 = 1;
                else
                    LED_BLANCHE_1 = 0;
            }

            if (payload[0] == '2') {
                if (payload[1] == '1')
                    LED_BLEUE_1 = 1;
                else
                    LED_BLEUE_1 = 0;
            }

            if (payload[0] == '3') {
                if (payload[1] == '1')
                    LED_ORANGE_1 = 1;
                else
                    LED_ORANGE_1 = 0;
            }

            if (payload[0] == '4') {
                if (payload[1] == '1')
                    LED_ROUGE_1 = 1;
                else
                    LED_ROUGE_1 = 0;
            }

            if (payload[0] == '5') {
                if (payload[1] == '1')
                    LED_VERTE_1 = 1;
                else
                    LED_VERTE_1 = 0;
            }
            break;
            
      
            
        
        case SET_ROBOT_STATE:
            SetRobotState(payload[0]);
            break;
        case SET_ROBOT_MANUAL_CONTROL:
            SetRobotAutoControlState(payload[0]);
            
            break;
        case COMMAND_PID_MOTEUR:

            SetupPidAsservissement(&robotState.PidX,getFloatFromBytes(payload,20),getFloatFromBytes(payload,24),getFloatFromBytes(payload,28),10,400,0);
            SetupPidAsservissement(&robotState.PidTheta,getFloatFromBytes(payload,8),getFloatFromBytes(payload,4),getFloatFromBytes(payload,0),10,400,0);
            UartEncodeAndSendMessage(COEFFICIENT_PID_VERIFY,payloadLength,payload);
            PWMSetSpeedConsignePolaire(getFloatFromBytes(payload,12),getFloatFromBytes(payload,16));
         
            
            a=getFloatFromBytes(payload,20);
            b=getFloatFromBytes(payload,24);
            c=getFloatFromBytes(payload,28);
            break;
           
        case ROTATION_GHOST: 
            etapeghost=ROTATION;
            Rotation.X = getFloatFromBytes(payload,0);
            Rotation.Y = getFloatFromBytes(payload,4);
            Rotation.ecart =getFloatFromBytes(payload,8);
            
         
            
            
            longitunal.HypoWay = sqrt(Rotation.X*Rotation.X +Rotation.Y*Rotation.Y);
          
         
            
              if(Rotation.X==0){
                  
                if(Rotation.Y<0)
                {
                    Rotation.ThetaWay=-M_PI/2- Rotation.ecart ;
                    Rotation.ecart = -M_PI/2;
            
                }
                    else{
                       Rotation.ThetaWay=M_PI/2 - Rotation.ecart  ;
                        Rotation.ecart = M_PI/2;
                    }
                 
                
            }
                    
         
  
            if(Rotation.X>0){
                 Rotation.ThetaWay=atan(Rotation.Y/Rotation.X) - Rotation.ecart;
                Rotation.ecart = atan(Rotation.Y/Rotation.X);
            }
               
            
            else if(Rotation.X<0){
                if(Rotation.Y>0)
                {
                     Rotation.ThetaWay=M_PI+atan(Rotation.Y/Rotation.X)- Rotation.ecart;
                   Rotation.ecart=M_PI+atan(Rotation.Y/Rotation.X);
                }
                   
                else{
                    Rotation.ThetaWay=-M_PI+atan(Rotation.Y/Rotation.X - Rotation.ecart);
                     Rotation.ecart =-M_PI+atan(Rotation.Y/Rotation.X);
                }
     
            }
           
           Waypoint();
   double R=40;
            B.x = R * cos(Rotation.ecartangle);
            B.y = R * sin(Rotation.ecartangle);
        /*    if ( Rotation.ecartangle == M_PI /2 | Rotation.ecartangle == -M_PI /2 ){
                     B.x=40;
                     B.y= sqrt((Rotation.X*Rotation.X)+(Rotation.Y*Rotation.Y));
            }
            else if (Rotation.ecartangle == M_PI | Rotation.ecartangle == 0){
                B.y=40;
                B.x= sqrt((Rotation.X*Rotation.X)+(Rotation.Y*Rotation.Y));
            }
            else
            {
                B.y=40;
                double save_angle =tan(Rotation.ecartangle);
                B.x= B.y/tan(Rotation.ecartangle);
                        
            }*/          
  break;
  
        case COMMAND_PD_ANG:
              SetupPidAsservissement(&PDAng,getFloatFromBytes(payload,0),0,getFloatFromBytes(payload,4),10,400,200);
    
            
            
            break;
        default:
            break;








    }




}


void SetRobotState(unsigned char state){
      switch(state){
        case STATE_ATTENTE : // case 0
            stateRobot = STATE_ATTENTE;
        case STATE_AVANCE : // case 2
            stateRobot = STATE_AVANCE;
            break;
        case STATE_TOURNE_GAUCHE : // case 4
            stateRobot = STATE_TOURNE_GAUCHE;
            break;
        case STATE_TOURNE_DROITE : // case 6
            stateRobot = STATE_TOURNE_DROITE;
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE : // case 8
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE : // case 10
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case STATE_ARRET : // case 12
            stateRobot = STATE_ARRET;
        case STATE_RECULE : // case 14
            stateRobot = STATE_RECULE;
            break;
        default :
            stateRobot = STATE_ATTENTE;
            break;
    }         
        
    
}

void SetRobotAutoControlState(unsigned char state) {
    if(state == 0){
        autoControlActivated=0;
        stateRobot = STATE_ATTENTE;
    }
    else
        autoControlActivated=1;//used dans timer
}