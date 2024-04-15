/*
 * File:   main_uart2.c
 * Author: Family
 *
 * Created on April 12, 2024, 10:50 AM
 */


#include "xc.h"
#include "timer.h"
#include "stdio.h"
#include "string.h"


uint16_t button_E8;
uint16_t button_E9;

uint16_t missed_timer;
uint16_t recived_char;
uint16_t tpf_led_blink;

void __attribute__((__interrupt__, no_auto_psv__)) _INT1Interrupt(void){
    IEC1bits.INT1IE = 0; // ENABLE to zero
    IEC1bits.INT2IE = 0;
    
    // when a button press is detected we block BOTH button, is in line whith 
    // the assignement to avoid the conflict of both button using the uart
    IFS1bits.INT1IF = 0; // reset the flag of the corresponding button
    //LATAbits.LATA0 = (!LATAbits.LATA0);
    button_E8 = 1;
    
    //TMR1 = 0;
    //IFS0bits.T1IF = 0; // reset flag timer
    //IEC0bits.T1IE = 1; 
}

void __attribute__((__interrupt__, no_auto_psv__)) _INT2Interrupt(void){
    IEC1bits.INT1IE = 0; // ENABLE to zero
    IEC1bits.INT2IE = 0;
    
    IFS1bits.INT2IF = 0; // reset the flag of the corresponding button
    //LATGbits.LATG9 = (!LATGbits.LATG9);
    button_E9 = 1;
    
    //TMR1 = 0;
    //IFS0bits.T1IF = 0; // reset flag timer
    //IEC0bits.T1IE = 1; 
}

void __attribute__((__interrupt__, no_auto_psv__)) _T1Interrupt(){
    IFS0bits.T1IF = 0; // reset flag timer
    //IEC0bits.T1IE = 0; // reset enable timer
    
    // FIRST USE OF TIMER1, NOW IS THE 10ms TIMER
   /* if (PORTEbits.RE8 == 1){ // if the flag is to 1                   
        IEC1bits.INT1IE = 1; // re enable the button interrupts
        IEC1bits.INT2IE = 1;
    }
    
    if (PORTEbits.RE9 == 1){
        IEC1bits.INT1IE = 1;
        IEC1bits.INT2IE = 1;
    }*/
    tpf_led_blink = tpf_led_blink + 1; //to make LD2 blink at 2.5Hz
    // first method only 1 enable is restored in each statements
}

/*the logic of this bouncing avoidance is as follows: when a button is pressed and the
 * related INTERR is triggered we BLOCK the reception for BOTH button. The button interrupts
 * toggles the corresponding led and rester the TRMx, FLAG and ENAB of the timer.
 * 
 * The timer is never stopped and in the TMRx interrupts the FLAG and ENABLE are 
 * resetted again.
 * 
 * PROBLEM:
 * if the button are held down and only one is released the ENAB of the not RESTORED
 * ending in a situation were said button INTERR is not working anymore
 * RESETTING BOTH BUTTON ENAB SOLVES THE PROBLEM, is not clear if the bouncing is 
 * avoided by doing so.
 */

// data implementazione dell UART sistema antiBOUNCE con timer inutile


void __attribute__((__interrupt__, no_auto_psv__))_U1RXInterrupt(void){
    IFS0bits.U1RXIF = 0; // reset INTERR flag to 0
    recived_char = recived_char + 1; // increase number of recived char
}

void __attribute__((__interrupt__, no_auto_psv__))_U1TXInterrupt(void){
    IFS0bits.U1TXIF = 0; // reset the flag of the TX reg
    
    // since the TX reg is now empty we can restore the button
    IEC1bits.INT1IE = 1;
    IEC1bits.INT2IE = 1;
}

void send_char(char carattere){
    U1TXREG = carattere;
}

void send_string(uint16_t button, uint16_t valore){
    char send_string[100];
    
    switch(button){
        case 1: sprintf(send_string, "C=%d", valore);break;
        case 2: sprintf(send_string, "D=%d", valore);break;
    }
    
    for(uint16_t i = 0; i < strlen(send_string); i++){
        send_char(send_string[i]);
    }
    //IEC1bits.INT1IE = 1;
    //IEC1bits.INT2IE = 1;
    //LATAbits.LATA0 = (!LATAbits.LATA0);
}

void log_string(char *send_string){
    for(uint16_t i = 0; i < strlen(send_string); i++){
        send_char(send_string[i]);
    }
}

void algo(){
    tmr_wait_ms(TIMER2, 7);
    // fake 7ms algoritm
}

int command_recognition(char *command_string){
    char header[3] = "LED";
    uint16_t control = 1;
    
    for (uint16_t i = 0; i<3; i++){
        if(command_string[i] != header[i]){
            control = 0;
            break;
        }
    }
    
    if (control == 1){
        switch(command_string[3]){
            case '1': return 1; break;
            case '2': return 2; break;
            default: return 0; break;
        }
    }
}

int main(void) {
    // disable analog pin
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    
    // UART SET UP
    RPINR18bits.U1RXR = 0x4b; // the input needs to be remapped to a particular pin
                              // as for the BUTTON to INTERR remapping (RX reg)
    RPOR0bits.RP64R = 1;      // on the other hand the output only needs to be "activated"
                              // since is a DATA output not a SINGNAL (TX reg)
    
    U1BRG = 468; // set baude rate
    U1MODEbits.UARTEN = 1; // enable uart COMMUNICATION --> similar to GEN ENAB
    U1STAbits.UTXEN = 1; // enable U1TX --> trasmission
    
    // TX reg INTERR
    U1STAbits.URXISEL0 = 1; 
    U1STAbits.URXISEL1 = 0; 
    // setting the TX interr to trigger when all the trasmission have occurred, than i can 
    // re enable the buttons
    IEC0bits.U1TXIE = 1; // enable the TX interrupt
    
    // RX reg INTERR
    U1STAbits.URXISEL0 = 0; // RX interr set to trigger for every char recived
    U1STAbits.URXISEL1 = 0;
    
    IEC0bits.U1RXIE = 1; // RX interr enable
    
    // LED SET UP
    TRISGbits.TRISG9 = 0; // output
    TRISAbits.TRISA0 = 0; // output
    LATGbits.LATG9 = 0; // start in OFF state
    LATAbits.LATA0 = 0; // start in OFF state
    
    // BUTTON SET UP
    TRISEbits.TRISE8 = 1; // input
    TRISEbits.TRISE9 = 1; // input
    
    RPINR0bits.INT1R = 0x58; // remap E8 button to EXT INTERR INT1R
    RPINR1bits.INT2R = 0x59; // remap E9 button to EXT INTERR INT2R
    // ENABLE and FLAG initialization
    INTCON2bits.GIE = 1; // general interrupt enable
    
    IFS1bits.INT1IF = 0; // FLAG to zero
    IFS1bits.INT2IF = 0;
    
    IEC1bits.INT1IE = 1; // ENABLE to one
    IEC1bits.INT2IE = 1;
    
    tmr_setup_period(TIMER1, 10, 1); // bouncing avoidance timer 
    
    button_E8 = 0;
    button_E9 = 0;
    
    missed_timer = 0;
    recived_char = 0; // increase every time a CHAR is recived
    tpf_led_blink = 0; // reset to zero every time is at 20
    
    
    uint16_t wp_ret = 0;
    uint16_t new_char = 0; // if the recived_char is greater a new char is present
    uint16_t str_pos = 0;
    uint16_t comm_action = 0;
    uint16_t ld2_active = 1;
    char command[5];
    command[5] = '\0';
    
    
    while(1){
        algo(); // 7ms delay
        
        if(recived_char > new_char){
            command[str_pos] = U1RXREG;
            str_pos++;
            new_char++;
            
            if(strlen(command) > 3){
                comm_action = command_recognition(command);
                log_string(command);
                str_pos = 0;
            }
        }
        
        if (comm_action == 1){
            LATAbits.LATA0 = (!LATAbits.LATA0);
            comm_action = 0;
        }else if(comm_action == 2){
            ld2_active = (!ld2_active);
            comm_action = 0;
        }
        
        if(tpf_led_blink == 20 && ld2_active == 1){
            LATGbits.LATG9 = (!LATGbits.LATG9);
            tpf_led_blink = 0;
        }
        
        if (button_E8 == 1){
            //send_char('a');
            //send_string("C=", 11);
            //LATGbits.LATG9 = 1;// ----> per qualche ragione adesso questo led si 
                                // accende subito senza premere nessun tasto
            //temp = 11;
            //send_number(11);
            send_string(1, recived_char);
            //LATGbits.LATG9 = (!LATGbits.LATG9);
            button_E8 = 0;
        }
        
        if (button_E9 == 1){
            //send_char('b');
            //send_string("D=", 22);
            //LATAbits.LATA0 = 1; 
            
            //temp = 22;
            //send_number(22);
            send_string(2, missed_timer);
            //LATGbits.LATG9 = (!LATGbits.LATG9);
            button_E9 = 0;
        }
    
        wp_ret = tmr_wait_period(TIMER1);
        missed_timer = missed_timer + wp_ret;
        // this function returns the value of the TIMER flag upon entering the call, 
        // a 0 --> dedline not reached
        // a 1 --> dedline reached (and missed)
        
        // the problem is if the flag goes up and than we wait ANOTHER 10ms before checking
        // we would loose a "missed deadline"
    } // blocking the code
    
    return 0;
}
