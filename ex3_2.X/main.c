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
//the name of the interrupt function can be found in the INTERRPUT VECTORS table
void __attribute__((__interrupt__, no_auto_psv)) _INT1Interrupt(void){
    
    TMR2 = 0; // reset timer
    IFS0bits.T2IF = 0; // set timer flag to 0 --> to read next interrupt
    
    while(1){
        while(tmr_wait_period(TIMER2) == 0){}
        if(IFS1bits.INT1IF == 0){
            continue;
        }else{
            break;
        } // tasto ancora premuto continue
    }
    
    LATGbits.LATG9 = (!LATGbits.LATG9);
}


// first exe main
int main(void) {
   ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0X0000;
    // disable analog pins
    
    // led initialization
    TRISAbits.TRISA0 = 0; // set led1 to output
    TRISGbits.TRISG9 = 0; //set led2 to output
    TRISEbits.TRISE8 = 1; //set left button to input
    
    LATAbits.LATA0 = 0;
    LATGbits.LATG9 = 0;
    
    RPINR0bits.INT1R = 0x58; // remapping of pin to interrupt
    INTCON2bits.GIE = 1;  // general interrupt enable
    IFS1bits.INT1IF = 0; // i flag
    IEC1bits.INT1IE = 1; // i enable
 
    
    
    // fronte salita della flag
    int controllo = 0; 
    
    // init timer 1
    tmr_setup_period(TIMER1, 200, 0);
    tmr_setup_period(TIMER2, 50, 0);
    
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
} 
 
