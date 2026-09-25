#include <xc.h>
#include "timer.h"
#include "IO.h"
#include"PWM.h"
#include"ADC.h"
#include "main.h"
#include "UART_Protocol.h"
#include "QEI.h"
#include "asservissement.h"
#include "Ghost.h"
//Initialisation d?un timer 16 bits
 unsigned long timestamp=0;
 float FCY = 60000000;
 int counterQ=0;

void InitTimer1(void) {
    //Timer1 pour horodater les mesures (1ms)
    T1CONbits.TON = 0; // Disable Timer
    //T1CONbits.TCKPS = 0b10; //Prescaler
    //11 = 1:256 prescale value
    //10 = 1:64 prescale value
    //01 = 1:8 prescale value
    //00 = 1:1 prescale value
    T1CONbits.TCS = 0; //clock source = internal clock
    //PR1 = 0x249F;
    SetFreqTimer1(2000);
    IFS0bits.T1IF = 0; // Clear Timer Interrupt Flag
    IEC0bits.T1IE = 1; // Enable Timer interrupt
    T1CONbits.TON = 1; // Enable Timer
    //page12 partie1 , f= FCY/(PS)*PR1 lors des calculs PR1==3KHz au lieu de 6khz (c est normal car c est le temps pour une demi periode)
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    IFS0bits.T1IF = 0;     
    PWMUpdateSpeed();
    ADC1StartConversionSequence();
   QEIUpdateData();  
    UpdateAsservissement();
  
    
    
    
     Distance_to_waypoint();
    //SendPositionData();
    if(counterQ++%15==0){
  
    //TransmitAsserv();
        
    }
  
}
//Interruption du timer 1
void InitTimer4(void) {
    //Timer1 pour horodater les mesures (1ms)
    T4CONbits.TON = 0; // Disable Timer
    //T4CONbits.TCKPS = 0b00; //Prescaler
    //11 = 1:256 prescale value
    //10 = 1:64 prescale value
    //01 = 1:8 prescale value
    //00 = 1:1 prescale value
    T4CONbits.TCS = 0; //clock source = internal clock
    //PR4 = 0xEA60;
    SetFreqTimer4(1000  );
    IFS1bits.T4IF = 0; // Clear Timer Interrupt Flag
    IEC1bits.T4IE = 1; // Enable Timer interrupt
    T4CONbits.TON = 1; // Enable Timer
    //page12 partie1 , f= FCY/(PS)*PR1 lors des calculs PR1==3KHz au lieu de 6khz (c est normal car c est le temps pour une demi periode)
}

void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void) {
    IFS1bits.T4IF = 0;  
  
    //timestamp++;
    //OperatingSystemLoop();
   EtatGhost();
     
      
       if (counterQ >= 25)
{
    counterQ = 0;
    Send_Ghost();
}


}


//Initialisation d?un timer 32 bits

void InitTimer23(void) {
    T3CONbits.TON = 0; // Stop any 16-bit Timer3 operation
    T2CONbits.TON = 0; // Stop any 16/32-bit Timer3 operation
    T2CONbits.T32 = 1; // Enable 32-bit Timer mode
    T2CONbits.TCS = 0; // Select internal instruction cycle clock
    T2CONbits.TCKPS = 0b00; // Select 1:1 Prescaler
    TMR3 = 0x00; // Clear 32-bit Timer (msw)
    TMR2 = 0x00; // Clear 32-bit Timer (lsw)
    PR3 = 0x1566; // Load 32-bit period value (msw)
    PR2 = 0x1566; // Load 32-bit period value (lsw) //clignotement en 0,5Hz or horloge de 60MHz donc cherche un facteur par lequel diviser 60Mhz pour obtenir 0,5Hz et ce facteur =120Mhz en hexa 727 0E00 la premiere horloge prends les trois premiers octets et la deuxieme prends les 4 derniers.
    IPC2bits.T3IP = 0x01; // Set Timer3 Interrupt Priority Level
    IFS0bits.T3IF = 0; // Clear Timer3 Interrupt Flag
    IEC0bits.T3IE = 1; // Enable Timer3 interrupt
    T2CONbits.TON = 1; // Start 32-bit Timer
}


//Interruption du timer 32 bits sur 2-3
unsigned char toggle = 0;

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {
    IFS0bits.T3IF = 0; // Clear Timer3 Interrupt Flag
   

}

void SetFreqTimer1(float freq) {
    T1CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
    if (FCY / freq > 65535) {
        T1CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
        if (FCY / freq / 8 > 65535) {
            T1CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
            if (FCY / freq / 64 > 65535) {
                T1CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                PR1 = (int) (FCY / freq / 256);
            } else
                PR1 = (int) (FCY / freq / 64);
        } else
            PR1 = (int) (FCY / freq / 8);
    } else
        PR1 = (int) (FCY / freq);
}

void SetFreqTimer4(float freq) {
    T4CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
    if (FCY / freq > 65535) {
        T4CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
        if (FCY / freq / 8 > 65535) {
            T4CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
            if (FCY / freq / 64 > 65535) {
                T4CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                PR4 = (int) (FCY / freq / 256);
            } else
                PR4 = (int) (FCY / freq / 64);
        } else
            PR4 = (int) (FCY / freq / 8);
    } else
        PR4 = (int) (FCY / freq);
}