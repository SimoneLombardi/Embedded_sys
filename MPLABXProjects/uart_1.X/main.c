/*
 * File:   mian.c
 * Author: Family
 *
 * Created on April 11, 2024, 2:38 PM
 */


#include "xc.h"

#define FCY 72000000
#define BAUDRATE 9600

void __attribute__((__interrupt__, no_auto_psv__))_U1RXInterrupt(){
    char carattere;
    
    IFS0bits.U1RXIF=0;
    carattere=U1RXREG; // read char recived
    U1TXREG=carattere; // retransmit the same char
    LATAbits.LATA0 = (!LATAbits.LATA0);
    //Flag raised each time a character is received 
    U1STAbits.URXISEL=0;
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
    
    IEC0bits.U1RXIE=1; //UART1 interrupt enable for available data
    
    //led for debugging
    TRISAbits.TRISA0 = 0;
    
    while(1){}
    
    return 0;
}
