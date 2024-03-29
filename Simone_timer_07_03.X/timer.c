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
            IFS0bits.T1IF = 0; // flag to zero
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
            IFS0bits.T2IF = 0; // flag to zero 
            break;
    }
}


int tmr_wait_period(int timer){
    int ret_val;
    switch(timer){
        case 1:
            ret_val = IFS0bits.T1IF;// save flag value before reset
            // set flag bit to 0 --> flag goes to one when timer expire
            IFS0bits.T1IF = 0; 
            // reset the timer and start watching the flag
            TMR1 = 0;

            break;
        case 2:
            ret_val = IFS0bits.T2IF; // save flag value before reset
            // set flag bit to 0 --> flag goes to one when timer expire
            IFS0bits.T2IF = 0; 
            // reset the timer and start watching the flag
            TMR2 = 0;

            break;
    }
    return ret_val;
}

/* -- EXE 2 wait_period version --
 * void tmr_wait_period(int timer){
    switch(timer){
        case 1:
            // set flag bit to 0 --> flag goes to one when timer expire
            IFS0bits.T1IF = 0; 
            // reset the timer and start watching the flag
            TMR1 = 0;

            break;
        case 2:
            // set flag bit to 0 --> flag goes to one when timer expire
            IFS0bits.T2IF = 0; 
            // reset the timer and start watching the flag
            TMR2 = 0;

            break;
    }
}
 */


void tmr_wait_ms(int timer, int ms){
    int resto = ms;
        
    while((resto - 200) > 0){
        tmr_setup_period(timer, 200);
        switch(timer){
        case 1:
            TMR1 = 0; // reset timer
            
            while(IFS0bits.T1IF == 0){ // wait timer termination
                continue;
            }
            IFS0bits.T1IF = 0; // reset flag
            T1CONbits.TON = 0; // stop the timer
            break;
        case 2:
            TMR2 = 0; // reset timer
            
            while(IFS0bits.T2IF == 0){ // wait timer termination
                continue;
            }
            IFS0bits.T2IF = 0; // reset flag
            T2CONbits.TON = 0; // stop the timer
            break;
        }
        resto = resto - 200; // update the time
    }
    
    tmr_setup_period(timer, resto);
    switch(timer){
        case 1:
            TMR1 = 0; // reset timer
            
            while(IFS0bits.T1IF == 0){ // wait timer termination
                continue;
            }
            IFS0bits.T1IF = 0; // reset flag
            T1CONbits.TON = 0; // stop the timer
            break;
        case 2:
            TMR2 = 0; // reset timer
            
            while(IFS0bits.T2IF == 0){ // wait timer termination
                continue;
            }
            IFS0bits.T2IF = 0; // reset flag
            T2CONbits.TON = 0; // stop the timer
            break;
        }
}


    
    /* -- EXE 2 wait_ms version --
     tmr_setup_period(timer, 200);
     
    switch(timer){
        case 1:
            TMR1 = 0; // reset timer
            
            while(IFS0bits.T1IF == 0){ // wait timer termination
                continue;
            }
            IFS0bits.T1IF = 0; // reset flag
            T1CONbits.TON = 0; // stop the timer
            break;
        case 2:
            TMR2 = 0; // reset timer
            
            while(IFS0bits.T2IF == 0){ // wait timer termination
                continue;
            }
            IFS0bits.T2IF = 0; // reset flag
            T2CONbits.TON = 0; // stop the timer
            break;
    }*/