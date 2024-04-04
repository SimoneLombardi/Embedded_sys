/*
 * File:   interrupts.c
 * Author: utente
 *
 * Created on March 20, 2024, 5:02 PM
 */


#include "xc.h"
#include "interrupt.h"


void tmr_setup_period(int timer, int ms){
    
    int prescaler_list[] = {1, 8, 64, 256};   // List of available prescaler
    long clk_freq = 72000000;                 // Frequency of clock
    int ms_in_sec = 1000/ms;                  // Number of ms in one second
    int time = -1;                            // Storage amount of time to raeach
    long time_to_reach = -1;                 
    int save_prescaler;
    int bit_prescaler_a;
    int bit_prescaler_b;
    
    
    // To compute the amount of time to reach: freq/sec/prescaler
    for (int i=0;i<4;i++){
        time_to_reach = clk_freq/(ms_in_sec/prescaler_list[i]);
        if (time_to_reach < 65535){
            time = time_to_reach;
            save_prescaler = prescaler_list[i];    // Get right value of prescaler CHIEDERE
            break;
        }
    }
    
    // Get value associated to prescaler
    if (save_prescaler == 256){
        bit_prescaler_a = 1;
        bit_prescaler_b = 1;
    }
    if (save_prescaler == 64){
        bit_prescaler_a = 1;
        bit_prescaler_b = 0;
    }
    if (save_prescaler == 8){
        bit_prescaler_a = 0;
        bit_prescaler_b = 1;
    }
    if (save_prescaler == 1){
        bit_prescaler_a = 0;
        bit_prescaler_b = 0;
    }
    
    switch (timer){
        case 1:
            T1CONbits.TCS = 0;              // Set clock source to internal (72 MHz)
            
            T1CONbits.TCKPS0 = bit_prescaler_a;    // Set prescaler bit
            T1CONbits.TCKPS1 = bit_prescaler_b;    // Set prescaler bit
            
            PR1 = time;                     // Amount of time to reach
            
            T1CONbits.TON = 1;              // Start timer
            IFS0bits.T1IF = 0;              // Flag set to zero
            
            break;
        
        case 2:
            
            T2CONbits.TCS = 0;              // Set clock source to internal (72 MHz)
            
            T2CONbits.TCKPS0 = bit_prescaler_a;    // Set prescaler
            T2CONbits.TCKPS1 = bit_prescaler_b;
            
            PR2 = time;                     // Amount of time to reach
            
            T2CONbits.TON = 1;              // Start timer
            IFS0bits.T2IF = 0;              // Flag set to zero
            
            break;
    }   
}


// Function useful for ex.1
void tmr_wait_period(int timer){
    switch (timer){
        case 1:
            while(1){
                // Check if the flag is up
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
                // Check if the flag is up
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