/*
 * File:   main_2103.c
 * Author: Family
 *
 * Created on March 21, 2024, 3:08 PM
 */


#include "xc.h"
#include "timer_2103.h"

int main(void) {
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0X0000;
    // disable analog pins
     
    // led initialization
    TRISAbits.TRISA0 = 0; // set to output led 1
    LATAbits.LATA0 = 0;
            
    tmr_setup_period(TIMER1, 100, 0);  // 2.5 Hz timer 1, no interrupt
    
    while(1){
        if(tmr_wait_period(TIMER1)){
            LATAbits.LATA0 = (!LATAbits.LATA0);
        }
    }
    
    return 0;
}
