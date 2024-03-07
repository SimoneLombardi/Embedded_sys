/*
 * File:   main.c
 * Author: enric
 *
 * Created on March 7, 2024, 12:02 PM
 */


#include "xc.h"

int main(void) {
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0X0000;
    // disable analog pins
    
    // led initialization
    TRISAbits.TRISA0 = 0; // set to output
    
    // init timer 1
    tmr_setup_period(TIMER1, 200);
    
    while(1){
        // turn led ON and wait
        LATAbits.LATA0 = 1;
        
        tmr_wait_period(TIMER1);
        
        // turn led OFF and wait
        LATAbits.LATA0 = 0;
        
        tmr_wait_period(TIMER1);
    }
    
    return 0;
}
