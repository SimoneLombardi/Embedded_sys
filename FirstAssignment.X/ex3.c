/*
 * File:   ex3.c
 * Author: utente
 *
 * Created on 27 febbraio 2024, 16.47
 */


#include "xc.h"

int main3(void) {
    
    // all analog pins disabled
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0X0000;
    
    int T2;
    int count = 0;          // how many times is press the button
    TRISEbits.TRISE8 = 1;   // set button as input
    TRISAbits.TRISA0 = 0;   // set led as output
    
    while(1){
        T2 = PORTEbits.RE8; // read if button is pressed
        
        if(T2 == 1){    // if button is pressed
            if (count == 0){
                LATAbits.LATA0 = 1; // turn on led
                count ++;
            }
            else if (count == 1){
                LATAbits.LATA0 = 0; // turn off led
                count = 0;
            }
        }
}
    
    return 0;
}
