/*
 * File:   main.c
 * Author: enric
 *
 * Created on March 21, 2024, 3:44 PM
 */


#include "xc.h"

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
    return 0;
}
