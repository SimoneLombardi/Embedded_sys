/*
 * File:   ex2.c
 * Author: utente
 *
 * Created on February 27, 2024, 1:15 PM
 */


#include "xc.h"

int main2(void) {
    
    // all analog pins disabled
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0X0000;
    
    int T2;
    TRISEbits.TRISE8 = 1;   // set button as input
    TRISAbits.TRISA0 = 0;   // set led as output
    
    while(1){
        T2 = PORTEbits.RE8; 
        if(T2 == 1){
            LATAbits.LATA0 = 1;
        }
        else{
            LATAbits.LATA0 = 0;
        }
        
    }
    return 0;
}
