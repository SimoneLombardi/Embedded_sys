/*
 * File:   main.c
 * Author: enric
 *
 * Created on March 21, 2024, 3:44 PM
 */


#include "xc.h"

void __attribute__((__interrupt__, no_auto_psv__))_U1RXInterrupt(){
    char carattere;
    IFS0bits.U1RXIF=0;
    carattere=U1RXREG;
    U1TXREG=carattere;
    LATAbits.LATA0 = (!LATAbits.LATA0);
    //Flag raised each time a character is received 
    U1STAbits.URXISEL=0;
}

int main(void) {
    //pin mapping
    //mapping of remappable pin RP64 on pin RD0 to work as UART1TX
    RPOR0bits.RP64R=1; 
    //mapping of remappable pin RPI75 on pin RD11 to work as UA1RX
    RPINR18bits.U1RXR=0x4b; // 0x4b is 75 in hexadecimal
    //UART1 init
    //BaudRate selection
    U1BRG=468; //(72MHz/(16*9600))-1
    U1MODEbits.UARTEN=1; //global UART enable
    U1STAbits.UTXEN=1; //UART1 enable
    IEC0bits.U1RXIE=1; //UART1 interrupt enable
    //led for debugging
    TRISAbits.TRISA0 = 0; 
    while(1){

    }
    return 0;
}
