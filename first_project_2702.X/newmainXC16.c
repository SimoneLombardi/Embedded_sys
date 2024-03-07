/*
 * File:   newmainXC16.c
 * Author: Family
 *
 * Created on February 27, 2024, 11:12 AM
 */

#include "xc.h"

int main(void) {
    // disable analog pin
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    
    int pin_value;
    
    TRISAbits.TRISA0 = 0; // set the pin as output
    
    TRISEbits.TRISE8 = 1; // set the button to input
    
    while(1){
        //LATAbits.LATA0 = 1; // turn on led LD1
        /*
        if((pin_value = PORTEbits.RE8) == 0){// 0 --> button pressed, 1 --> button released
            LATAbits.LATA0 = 1; // turn on led LD1
        }else{
            LATAbits.LATA0 = 0; // turn off led LD1
        }*/
    
        if((pin_value = PORTEbits.RE8) == 0){
            while((pin_value = PORTEbits.RE8) == 0){
                continue;
            }
            LATAbits.LATA0 = (!LATAbits.LATA0); // turn on led LD1
        }
    }
    return 0;
}
