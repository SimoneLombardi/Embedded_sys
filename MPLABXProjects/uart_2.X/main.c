/*
 * File:   main.c
 * Author: Family
 *
 * Created on April 11, 2024, 5:45 PM
 */


#include "xc.h"
#include "timer.h"
#include "interrupts.h"

int button_1 = 0;
int button_2 = 0;
int recived_char_count = 0;
int missed_timer = 0;
int tpf_blink = 0;

void __attribute__((__interrupt__, no_auto_psv)) _T3Interrupt(void){
    IFS0bits.T3IF = 0;      // Reset timer3 flag
    IEC0bits.T3IE = 0;      // Disable timer3 interrupt
    T3CONbits.TON = 0;      // Stop the timer
    
    if(IEC1bits.INT1IE == 0){ // --> button 1 is disabled, it been pressed
        if(IFS1bits.INT1IF == 1){ // --> if the flag is UP == bounce avoided
            button_1 = 1; // set global variable to 1 --> execute relative fnct in main
            
            IFS1bits.INT1IF = 0;    // Reset button flag
            IEC1bits.INT1IE = 1;    // ReEnable button
            
            LATGbits.LATG9 = (!LATGbits.LATG9);
        }
    }
    
    if(IEC1bits.INT2IE == 0){ // --> button 2 is disabled, it been pressed
        if(IFS1bits.INT2IF == 1){ // --> if the flag is UP == bounce avoided
            button_2 = 1; // set global variable to 1 --> execute relative fnct in main
            
            IFS1bits.INT2IF = 0;    // Reset button flag
            IEC1bits.INT2IE = 1;    // ReEnable button
            
            LATAbits.LATA0 = (!LATAbits.LATA0);
        }
    }
}

void __attribute__((__interrupt__, no_auto_psv__))_U1RXInterrupt(void){
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

void __attribute__((__interrupt__, no_auto_psv__)) _INT1Interrupt(void){
    
    IFS1bits.INT1IF = 0;            // Reset button flag
    IEC1bits.INT1IE = 0;            // Disable button
    tmr_setup_period(TIMER3, 20, 1);
}

void __attribute__((__interrupt__, no_auto_psv__)) _INT2Interrupt(void){
    
    IFS1bits.INT2IF = 0;            // Reset button flag
    IEC1bits.INT2IE = 0;            // Disable button
    tmr_setup_period(TIMER3, 20, 1);
}

void algoritm(){
    tmr_wait_ms(TIMER1, 7);
}


int main(void) {
    // disable analog pin
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    
    
    // uart set up code
    // INPUT REMAPPING 
    //TRISDbits.TRISD11 = 1; 
    RPINR18bits.U1RXR = 0x4b;
    // this is a 8 bit register and it is indexed entirely by this name U1RXR
    // we also need to set the corresponding TRIS bits to 1 -> input
    // OUTPUT REMAPPING 
    RPOR0bits.RP64R = 1;
    U1BRG = 468; // set baude rate
    U1MODEbits.UARTEN = 1; // enable uart COMMUNICATION
    U1STAbits.UTXEN = 1; // enable U1TX --> trasmission
    // data reception doesnt need to be enabled
    IEC0bits.U1RXIE = 1; //UART1 interrupt enable for available data
    
    //led set up
    TRISGbits.TRISG9 = 0;
    LATGbits.LATG9 = 0; // start from OFF state
    TRISAbits.TRISA0 = 0;
    LATAbits.LATA0 = 0; // start from OFF state
    
    //button set up
    TRISEbits.TRISE8 = 1;
    TRISEbits.TRISE9 = 1;
    // interrupts on button
    INTCON2bits.GIE = 1;        // General interrupt enable
    
    RPINR0bits.INT1R = 0x58;    // Remapping of pin to interrupt button 1 88 E8
    IFS1bits.INT1IF = 0;        // Interrupt flag
    IEC1bits.INT1IE = 1;        // Interrupt enable 
    
    RPINR1bits.INT2R = 0x59;    // Remapping of pin to interrupt button 2 89 E9
    IFS1bits.INT2IF = 0;        // Interrupt flag
    IEC1bits.INT2IE = 1;        // Interrupt enable
   
    tmr_setup_period(TIMER2, 10, 1); // assignment timer
    
    while(1){
        algoritm(); // simulating a 7ms algoritm
        
        tmr_wait_period(TIMER2);
        // this timer is only for control purpose, is not to force the 100Hz rate
    }
    
    return 0;
}