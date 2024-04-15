/*
 * File:   interrupts.c
 * Author: Family
 *
 * Created on 12 aprile 2024, 11.46
 */


#include "xc.h"
#include "timer.h"
#include "interrupts.h"


void __attribute__((__interrupt__, no_auto_psv__))_U1RXInterrupt(){
    char carattere;
    
    IFS0bits.U1RXIF = 0;
    carattere = U1RXREG; // read char recived
    U1TXREG = carattere; // retransmit the same char
    //Flag raised each time a character is received 
    U1STAbits.URXISEL = 0;
}

void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void){
    IFS0bits.T1IF = 0; // set timer flag to 0 --> to read next interrupt
}

void __attribute__((__interrupt__, no_auto_psv)) _T2Interrupt(void){
    IFS0bits.T2IF = 0; // set timer flag to 0 --> to read next interrupt
}

void __attribute__((__interrupt__, no_auto_psv__)) _INT1Interrupt(){
    
    IFS1bits.INT1IF = 0;            // Reset button flag
    IEC1bits.INT1IE = 0;            // Disable button
    tmr_setup_period(TIMER3, 20, 1);
}

void __attribute__((__interrupt__, no_auto_psv__)) _INT2Interrupt(){
    
    IFS1bits.INT2IF = 0;            // Reset button flag
    IEC1bits.INT2IE = 0;            // Disable button
    tmr_setup_period(TIMER3, 20, 1);
}


// -------------------- utility function --------------------------- //




