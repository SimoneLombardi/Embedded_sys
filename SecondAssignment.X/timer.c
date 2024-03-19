/*
 * File:   timer.c
 * Author: utente
 *
 * Created on 6 marzo 2024, 22.45
 */

#include <p33EP512MU810.h>

#include "timer.h"

void tmr_setup_period(int timer, int ms){
    
    //long clk_freq = 72000000;               // Frequency of clock
    //int ms_in_sec = 1000/ms;                // Number of ms in one second
    //int clk_steps = clk_freq/ms_in_sec;     // Number of clock steps in ms
    
    switch (timer){
        case 1:
            T1CONbits.TCS = 0;      // Set clock source to internal (72 MHz)
            
            T1CONbits.TCKPS0 = 1;   // Set the prescaler to 256 ((72MHz/0.2)/256 = 56250 < 65535 (16-bit))
            T1CONbits.TCKPS1 = 1;   // Remember that are two bits
            
            PR1 = (281*ms);    // Prescaler chosen 1:256
            
            T1CONbits.TON = 1;      // Start timer
            IFS0bits.T1IF = 0;      // Flag set to zero
            
            break;
        
        case 2:
            
            T2CONbits.TCS = 0;      // Set clock source to internal (72 MHz)
            
            T2CONbits.TCKPS0 = 1;   // Set the prescaler to 256 ((72MHz/0.2)/256 = 56250 < 65535 (16-bit))
            T2CONbits.TCKPS1 = 1;   // Remember that are two bits
            
            PR2 = (281*ms);    // Prescaler chosen 1:256
            
            T2CONbits.TON = 1;      // Start timer
            IFS0bits.T2IF = 0;      // Flag set to zero
            
            break;
    }   
}


void tmr_wait_period(int timer){
    switch (timer){
        case 1:
            while(1){
                if (IFS0bits.T1IF == 1){
                    IFS0bits.T1IF = 0;     // Set flag to zero
                    TMR1 = 0;               // Set timer to zero
                    break;
                }
                else{
                    continue;
                }
            }
            break;
        
        case 2:
            while(1){
                if (IFS0bits.T2IF == 1){
                    IFS0bits.T2IF = 0;     // Set flag to zero
                    TMR2 = 0;               // Set timer to zero
                    break;
                }
                else{
                    continue;
                }
            }
            break;
    }
}

/*
void tmr_wait_period(int timer){

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