/*
 * File:   main.c
 * Author: Family
 *
 * Created on March 7, 2024, 11:08 AM
 * 
 * main code of the project
 */


#include "xc.h"
#include "timer.h"


// first exe main
int main(void) {
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0X0000;
    // disable analog pins
    
    // led initialization
    TRISAbits.TRISA0 = 0; // set to output led 1
    TRISGbits.TRISG9 = 0; // set to output led 2
    
    LATAbits.LATA0 = 0;
    LATGbits.LATG9 = 0;
    
    while(1){
        LATAbits.LATA0 = 1;
        tmr_wait_ms(TIMER1, 50);
        LATAbits.LATA0 = 0;
        
        LATGbits.LATG9 = 1;
        tmr_wait_ms(TIMER1, 200);
        LATGbits.LATG9 = 0;
        
        LATAbits.LATA0 = 1;
        tmr_wait_ms(TIMER1, 2000);
        LATAbits.LATA0 = 0;
        
        tmr_wait_ms(TIMER1, 2000);
    }
    
    
    return 0;
} 
 
/*     -- EXE 1 --
 *   ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0X0000;
    // disable analog pins
    
    // led initialization
    TRISAbits.TRISA0 = 0; // set to output
    
    // fronte salita della flag
    int controllo = 0; 
    
    // init timer 1
    tmr_setup_period(TIMER1, 200);
    
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
    
    return 0;
 */

/*
 while(1){
        // turn led ON and wait
        LATAbits.LATA0 = 1;
            
        tmr_wait_ms(TIMER2, 20); // messe tante chiamate per vedere effetto su led
        tmr_wait_ms(TIMER2, 20);
        tmr_wait_ms(TIMER2, 20);
        tmr_wait_ms(TIMER2, 20);
        tmr_wait_ms(TIMER2, 20);
        tmr_wait_ms(TIMER2, 20);
 
 
        // turn led ON and wait
        LATAbits.LATA0 = 0;
            
        tmr_wait_ms(TIMER2, 200); // messe tante chiamate per vedere effetto su led
        tmr_wait_ms(TIMER2, 200);
        tmr_wait_ms(TIMER2, 200);
        tmr_wait_ms(TIMER2, 200);
        tmr_wait_ms(TIMER2, 200);
        tmr_wait_ms(TIMER2, 200);
    }
 */