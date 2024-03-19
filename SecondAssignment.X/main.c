/*
 * File:   main.c
 * Author: utente
 *
 * Created on March 6, 2024, 5:55 PM
 */


#include <p33EP512MU810.h>

#include "xc.h"
#include "timer.h"

int main(void) {
    
    // all analog pins disabled
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0X0000;
    
    TRISAbits.TRISA0 = 0;   // Set LD1 as ouptut
    int value = 0;          // Value for switch on or off the led
    
    tmr_setup_period(TIMER1, 200);
    
    /*
    // fronte salita della flag
    int controllo = 0; 
    
    while(1){
        if(IFS0bits.T1IF == 1){
            tmr_wait_period(TIMER1); // reset of timer and flag
            controllo = (!controllo);
        }
        
        if(controllo == 0){
            // turn led OFF and wait
            LATAbits.LATA0 = 0;
        }else{
            // turn led ON and wait
            LATAbits.LATA0 = 1;
        }
    }
    */
    while(1){
        LATAbits.LATA0 = value;
        tmr_wait_period(TIMER1);
        value = (!value);
    }
    return 0;
}
