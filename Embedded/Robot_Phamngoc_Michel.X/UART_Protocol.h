/* 
 * File:   UART_Protocol.h
 * Author: E306-PC2
 *
 * Created on 19 décembre 2025, 08:12
 */

#ifndef UART_PROTOCOL_H
#define	UART_PROTOCOL_H

unsigned char UartCalculateChecksum(int msgFunction,
int msgPayloadLength, unsigned char* msgPayload);
void UartEncodeAndSendMessage(int msgFunction,
int msgPayloadLength, unsigned char* msgPayload);
void UartDecodeMessage(unsigned char c);
void UartProcessDecodedMessage(int function,
int payloadLength, unsigned char* payload);
void SetRobotState(unsigned char state);
void SetRobotAutoControlState(unsigned char state);
extern int autoControlActivated;
#define Waiting 0
#define FunctionMSB 1

#define FunctionLSB  2
#define PayloadLengthMSB  3 
#define PayloadLengthLSB  4
#define Payload 5
#define CheckSum 6
#define SET_ROBOT_STATE 0x0051
#define SET_ROBOT_MANUAL_CONTROL 0x0052
#define COMMAND_PID_MOTEUR 0x0067
#define COEFFICIENT_PID_VERIFY 0x0068
#define CORR_PID_VARIABLES 0x0069
#define ROTATION_GHOST 0x0081
#define COMMAND_PD_ANG 0x0083
#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* UART_PROTOCOL_H */

