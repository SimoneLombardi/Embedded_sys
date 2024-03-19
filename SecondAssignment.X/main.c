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
    
    while(1){
        LATAbits.LATA0 = value;
        tmr_wait_period(TIMER1);
        value = (!value);
    }
    return 0;
}
