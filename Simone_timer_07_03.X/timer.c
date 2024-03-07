/*
 * File:   timer.c
 * Author: Family
 *
 * Created on March 7, 2024, 11:08 AM
 */


#include "xc.h"
#include "timer.h"

void tmr_setup_period(int timer, int ms){
    switch(timer){
        case 1:
            T1CONbits.TCS = 0; // set clock source to internal 72 MHz
    
            // setting the prescaler to 256 --> 14.400.000 / 256 = 56.250 < 65535
            T1CONbits.TCKPS0 = 1;
            T1CONbits.TCKPS1 = 1;

            PR1 = (281 * ms); 
            /*
             *the correct value is 14.400.000 / 200 / 256 = 281.25 but we can't count 
             *to that so we approximate the value to the next integer
             */

            T1CONbits.TON = 1; // start the timer
            break;
        case 2: 
            T2CONbits.TCS = 0; // set clock source to internal 72 MHz
    
            // setting the prescaler to 256 --> 14.400.000 / 256 = 56.250 < 65535
            T2CONbits.TCKPS0 = 1;
            T2CONbits.TCKPS1 = 1;

            PR2 = (282 * ms); 
            /*
             *the correct value is 14.400.000 / 200 / 256 = 281.25 but we can't count 
             *to that so we approximate the value to the next integer
             */

            T2CONbits.TON = 1; // start the timer
            break;
    }
}


void tmr_wait_period(int timer){

}


void tmr_wait_ms(int timer, int ms){

}
