/*
 * File:   timer_2103.c
 * Author: Family
 *
 * Created on March 21, 2024, 2:28 PM
 */


#include "xc.h"
#include "timer.h"

int tmr_compute_prescaler(int ms){
    if(tmr_counter_calculation(ms, 1) < TIMER_LIMIT){
        return 0;
    }else if(tmr_counter_calculation(ms, 8) < TIMER_LIMIT){
        return 1;
    }else if(tmr_counter_calculation(ms, 64) < TIMER_LIMIT){
        return 2;
    }else if(tmr_counter_calculation(ms, 256) < TIMER_LIMIT){
        return 3;
    }else{
        return 4;
    }
}

float tmr_counter_calculation(int ms, int prescaler){
    float return_value = (FCY / prescaler) * (ms / 1000.0);
    return return_value;
}

int tmr_convert_prescaler(int prescaler_bits){
    int return_value = 1;
    switch(prescaler_bits){
        case 0: return_value = 1;
                break;
                
        case 1: return_value = 8;
                break;
                
        case 2: return_value = 64;
                break;
                
        case 3: return_value = 256;
                break;
    }
     
    //return 256;
    return return_value;
}

void tmr_setup_period(int timer, int ms, int interr_act){
    int prescaler_bits;
    
    switch(timer){
        case 1:
            T1CONbits.TCS = 0; // set clock source to internal 72 MHz
    
            // setting the prescaler to 256 --> 14.400.000 / 256 = 56.250 < 65535
            //T1CONbits.TCKPS0 = 1;
            //T1CONbits.TCKPS1 = 1;
            
            //T1CONbits.TCKPS = 3; // set all the bits to 1
            
            //PR1 = (FCY / 256)*(ms/1000.0); 
            /*
             *the correct value is 14.400.000 / 200 / 256 = 281.25 but we can't count 
             *to that so we approximate the value to the next integer
             */
            prescaler_bits = tmr_compute_prescaler(ms);
            T1CONbits.TCKPS = prescaler_bits; // set all the bits to 1
            
            PR1 = (FCY / tmr_convert_prescaler(prescaler_bits))*(ms/1000.0); 
            
            if(interr_act == 1){
                IEC0bits.T1IE = 1; // activate enable for interrupts
            }

            T1CONbits.TON = 1; // start the timer
            IFS0bits.T1IF = 0; // flag to zero
            break;
        case 2: 
            T2CONbits.TCS = 0; // set clock source to internal 72 MHz
    
            // setting the prescaler to 256 --> 14.400.000 / 256 = 56.250 < 65535
            //T2CONbits.TCKPS0 = 1;
            //T2CONbits.TCKPS1 = 1;
            //T2CONbits.TCKPS = 3;
            
            //PR2 = (FCY / 256)*(ms/1000.0); 
            /*
             *the correct value is 14.400.000 / 200 / 256 = 281.25 but we can't count 
             *to that so we approximate the value to the next integer
             */
            prescaler_bits = tmr_compute_prescaler(ms);
            T2CONbits.TCKPS = prescaler_bits; // set all the bits to 1
            
            PR2 = (FCY / tmr_convert_prescaler(prescaler_bits))*(ms/1000.0); 
            
            if(interr_act == 1){
                IEC0bits.T2IE = 1; // activate enable for interrupts
            }

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
            if(ret_val == 1){
                // reset the timer and start watching the flag
                TMR1 = 0;
            }

            break;
        case 2:
            ret_val = IFS0bits.T2IF; // save flag value before reset
            // set flag bit to 0 --> flag goes to one when timer expire
            IFS0bits.T2IF = 0; 
            if(ret_val == 1){
                // reset the timer and start watching the flag
                TMR2 = 0;
            }

            break;
    }
    return ret_val;
}

void tmr_wait_ms(int timer, int ms){
    int resto = ms;
        
    while((resto - 200) > 0){
        tmr_setup_period(timer, 200, 0);
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
    
    tmr_setup_period(timer, resto, 0);
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
