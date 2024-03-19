/*
 * File:   main.c
 * Author: utente
 *
 * Created on February 27, 2024, 12:30 PM
 */


#include "xc.h"

int main(void) {
    // all analog pins disabled
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0X0000;
    
    /*
     * Ex. 1

    TRISAbits.TRISA0 = 0;   // set the pin as output
    LATAbits.LATA0 = 1;     // set the pin high
    while(1){}
    
    return 0;
}

*/

/*
 // Ex.2
    
    int T2;
    TRISEbits.TRISE8 = 1;   // set button as input
    TRISAbits.TRISA0 = 0;   // set led as output
    
    while(1){
        T2 = PORTEbits.RE8; 
        if(T2 == 1){
            LATAbits.LATA0 = 0;
        }
        else{
            LATAbits.LATA0 = 1;
        }
        
    }
    return 0;
}
*/

//Ex. 3
    
    int T2;
    int count = 0;          // how many times is press the button
    TRISEbits.TRISE8 = 1;   // set button as input
    TRISAbits.TRISA0 = 0;   // set led as output
    
    while(1){
        T2 = PORTEbits.RE8; // read if button is pressed
        
        if(T2 == 0){    // if button is pressed
            if (count == 0){
                LATAbits.LATA0 = 1; // turn on led
                count ++;
            }
            else if (count == 1){
                LATAbits.LATA0 = 0; // turn off led
                count = 0;
            }
        }
}
    return 0;
}