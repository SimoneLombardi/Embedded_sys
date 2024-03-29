/*
 * File:   main.c
 * Author: utente
 *
 * Created on March 20, 2024, 4:39 PM
 */


#include <p33EP512MU810.h>

#include "xc.h"
#include "interrupt.h"


void __attribute__((__interrupt__, __auto_psv__)) _T2Interrupt(){
    
    TMR2 = 0;               // Reset timer
    
    IFS0bits.T2IF = 0;      // Reset timer2 flag 
    
    LATGbits.LATG9 = (!LATGbits.LATG9);     // Change status led2
}


int main(void) {
    
    // all analog pins disabled
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0X0000;
    
    // Ex.1
    TRISAbits.TRISA0 = 0;   // Set LD1 as ouptut
    TRISGbits.TRISG9 = 0;   // Set LD2 as output
    int state_ld1 = 0;      // Value for switch on or off the LD1
    int state_ld2 = 0;      // Value for switch on or off the LD2
    
    tmr_setup_period(TIMER1, 200);      // Timer1
    tmr_setup_period(TIMER2, 100);      // Timer2 with interrupt enabled
    
    IEC0bits.T2IE = 1;      // Enable T2 interrupt
    
    
    while(1){
        LATAbits.LATA0 = state_ld1;    // Get value for switching status of LD1
        LATGbits.LATG9 = 0;            // Set to low the LED2
        tmr_wait_period(TIMER1);
        tmr_wait_period(TIMER2);
        state_ld1 = (!state_ld1);
    }
    
    return 0;
}