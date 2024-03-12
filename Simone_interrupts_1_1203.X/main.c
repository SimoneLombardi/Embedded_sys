/*
 * File:   main.c
 * Author: Family
 *
 * Created on March 12, 2024, 12:51 PM
 */

#include "xc.h"
#include "timer.h"

void __attribute__((__interrupt__, no_auto_psv)) _T2Interrupt(void){
    IFS0bits.T2IF = 0; // set timer flag to 0 --> to read next interrupt
    
    LATGbits.LATG9 = (!LATGbits.LATG9);
}

int main(void) {
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0X0000;
    // disable analog pins
     
    // led initialization
    TRISAbits.TRISA0 = 0; // set to output led 1
    TRISGbits.TRISG9 = 0; // set to output led 2
    
    LATAbits.LATA0 = 0;
    LATGbits.LATG9 = 0;
            
    tmr_setup_period(TIMER1, 200, 0);  // 2.5 Hz timer 1, no interrupt
    tmr_setup_period(TIMER2, 100, 1); // 5Hz timer 2, si interrupt
    
    int controllo = 0;
    
    while(1){
        /*if(tmr_wait_period(TIMER1)){
            LATAbits.LATA0 = (!LATAbits.LATA0);
        }*/
        
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
    
    return 0;
}


