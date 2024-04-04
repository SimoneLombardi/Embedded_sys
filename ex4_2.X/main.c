/*
 * File:   main.c
 * Author: enric
 *
 * Created on April 4, 2024, 5:24 PM
 */


#include "xc.h"
#include "timer.h"

void __attribute__((__interrupt__, no_auto_psv__)) _T3Interrupt(){
    
    IFS0bits.T3IF = 0;      // Reset timer2 flag 
    TMR3=0;
    LATAbits.LATA0 = (!LATAbits.LATA0);
}


void algorithm() {
    tmr_wait_ms(TIMER2, 7);
}
int main() {
    int ret;
    //LED2 as output
    TRISAbits.TRISA0 = 0;
    //LED2 init
    LATAbits.LATA0=0;
    tmr_setup_period(TIMER1, 10, 0); //togliere interrupt activation
    //timer for LED2 blinking
    tmr_setup_period(TIMER3, 100, 1); 
    while(1) {
        algorithm();
        // code to handle the assignment
        ret = tmr_wait_period(TIMER1);
    }
    return 0;
}
