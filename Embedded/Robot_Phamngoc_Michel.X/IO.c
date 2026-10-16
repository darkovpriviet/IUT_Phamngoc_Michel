
/*
 * File:   IO.c
 */

#include <xc.h>
#include "IO.h"

void InitIO()
{
    //****************************************************************************************************/
    // Declaration des pin Analogiques
    //****************************************************************************************************/
    ANSELA=0;             //Desactivation de toutes entree analogique
    ANSELB=0;             //Desactivation de toutes entree analogique
    ANSELD=0;             //Desactivation de toutes entree analogique
    ANSELC=0;             //Desactivation de toutes entree analogique
    ANSELE=0;             //Desactivation de toutes entree analogique
    ANSELG=0;             //Desactivation de toutes entree analogique
    
    
    //******************** QEI ***************** TP3
    _QEA2R = 97; //assign QEI A to pin RP97
    _QEB2R = 113; //assign QEI B to pin RP96
    _QEA1R = 126; //assign QEI A to pin RP70
    _QEB1R = 124; //assign QEI B to pin RP69


    // Configuration des sorties

    //******* LED ***************************
    _TRISJ6 = 0;  // LED Orange
    _TRISJ5 = 0; //LED Blanche
    _TRISJ4 = 0; // LED Bleue
    _TRISJ11 = 0; // LED Rouge
    _TRISH10 = 0; // LED Verte
    
    //**************Bouton *******************
    
    _TRISH0 = 1;
    _TRISH1 = 1;
    _TRISH2 = 1;
    
    //Prend les 2 derniers caractères avec _TRIS 
    _TRISA9 = 0; // LED Bleu
    _TRISA10 = 0; // LED Rouge 
    _TRISH3 = 0; // LED Vert
    _TRISK15 = 0; // LED Orange
     _TRISA0 = 0; // LED Blanche
    
    
     _TRISJ10 = 0; // EN PWM

    
    //****** Moteurs ************************

    // Configuration des entrées
    

    /****************************************************************************************************/
    // Gestion des pin remappables
    /****************************************************************************************************/
    UnlockIO(); // On unlock les registres d'entrées/sorties, ainsi que les registres des PPS
    _U1RXR = 78; //Remappe la RP... sur l?éentre Rx1
    _RP79R = 0b00001; //Remappe la sortie Tx1 vers RP... 
   //Assignation des remappable pins
        
    LockIO(); // On lock les registres d'entrées/sorties, ainsi que les registres des PPS
}


void LockIO() {
    asm volatile ("mov #OSCCON,w1 \n"
                "mov #0x46, w2 \n"
                "mov #0x57, w3 \n"
                "mov.b w2,[w1] \n"
                "mov.b w3,[w1] \n"
                "bset OSCCON, #6":: : "w1", "w2", "w3");
}

void UnlockIO() {
    asm volatile ("mov #OSCCON,w1 \n"
                "mov #0x46, w2 \n"
                "mov #0x57, w3 \n"
                "mov.b w2,[w1] \n"
                "mov.b w3,[w1] \n"
                "bclr OSCCON, #6":: : "w1", "w2", "w3");
}