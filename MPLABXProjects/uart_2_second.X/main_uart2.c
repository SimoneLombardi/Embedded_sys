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


uint16_t BUTTON_E8;
uint16_t BUTTON_E9;

uint16_t NEW_COMM;
uint16_t REC_CHAR;
uint16_t LED_BL_COUNTER; 

uint16_t COMM_INDX;
char COMMAND[5];
char RESET_STR[5] = {'\0'};



void __attribute__((__interrupt__, no_auto_psv__)) _INT1Interrupt(void){
    IEC1bits.INT1IE = 0; // ENABLE to zero
    IEC1bits.INT2IE = 0;
    
    // when a button press is detected we block BOTH button, is in line whith 
    // the assignement to avoid the conflict of both button using the uart
    IFS1bits.INT1IF = 0; // reset the flag of the corresponding button
    //LATAbits.LATA0 = (!LATAbits.LATA0);
    BUTTON_E8 = 1;
    
    //TMR1 = 0;
    //IFS0bits.T1IF = 0; // reset flag timer
    //IEC0bits.T1IE = 1; 
}

void __attribute__((__interrupt__, no_auto_psv__)) _INT2Interrupt(void){
    IEC1bits.INT1IE = 0; // ENABLE to zero
    IEC1bits.INT2IE = 0;
    
    IFS1bits.INT2IF = 0; // reset the flag of the corresponding button
    //LATGbits.LATG9 = (!LATGbits.LATG9);
    BUTTON_E9 = 1;
    
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
    LED_BL_COUNTER = LED_BL_COUNTER + 1; //to make LD2 blink at 2.5Hz
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
    
    COMMAND[COMM_INDX] = U1RXREG; // save the new char in the string
    COMM_INDX++; // increase position index
    REC_CHAR++;
    
    if (COMM_INDX == 4){ // if i have a complete command make the control
        NEW_COMM = 1;
    }
}

void __attribute__((__interrupt__, no_auto_psv__))_U1TXInterrupt(void){
    IFS0bits.U1TXIF = 0; // reset the flag of the TX reg
    
    // since the TX reg is now empty we can restore the button
    IEC1bits.INT1IE = 1;
    IEC1bits.INT2IE = 1;
}

// ----------------------------------- utilty function ------------------------------ //

void send_char(char carattere){
    U1TXREG = carattere;
}

void send_string(char *input_string){
    char toSend_str[100];
    
    sprintf(toSend_str, "%s", input_string);
    for (uint16_t i = 0; i < strlen(input_string); i++){
        send_char(toSend_str[i]);
    }
}

void algo_simul(){
    tmr_wait_ms(TIMER2, 7);
    // fake 7ms algoritm
}

void button_action(uint16_t button, uint16_t valore){
    char toSend_str[100];
    
    switch(button){
        case 1: sprintf(toSend_str, "C=%d", valore);break;
        case 2: sprintf(toSend_str, "D=%d", valore);break;
    }
    
    send_string(toSend_str);
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
            default: break;
        }
    }
    
    return 0;
}


int uart_setup(int TX_interrupt_type){
    // UART SET UP
    // this is fixed since we have a space problem on the car
    RPINR18bits.U1RXR = 0x4b; // the input needs to be remapped to a particular pin
                              // as for the BUTTON to INTERR remapping (RX reg)
    RPOR0bits.RP64R = 1;      // on the other hand the output only needs to be "activated"
                              // since is a DATA output not a SINGNAL (TX reg)
    
    // auto setting the baude rate
    U1BRG = 468;
    
    U1MODEbits.UARTEN = 1; // enable uart COMMUNICATION --> similar to GEN ENAB
    U1STAbits.UTXEN = 1; // enable U1TX --> trasmission
    
    // TX reg INTERR type selection
    if(TX_interrupt_type < 0 || TX_interrupt_type > 3){
        return 0;
    }else{
        U1STAbits.URXISEL = TX_interrupt_type;
        IEC0bits.U1TXIE = 1; // enable the TX interrupt only if setted
    }
    
    // RX reg INTERR, fixed on interrupt on single data recived
    U1STAbits.URXISEL0 = 0; // RX interr set to trigger for every char recived
    U1STAbits.URXISEL1 = 0;
    
    IEC0bits.U1RXIE = 1; // RX interr enable
    
    return 0;
}

int main(void) {
    // disable analog pin
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    
    uart_setup(9600, 1);
    
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
    
    // GLOBAL VARIABLE INITIALIZATION
    BUTTON_E8 = 0;
    BUTTON_E9 = 0;
    NEW_COMM = 0; // increase every time a CHAR is recived
    REC_CHAR = 0;
    LED_BL_COUNTER = 0; // reset to zero every time is at 20
    COMM_INDX = 0;
    COMMAND[4] = '\0';

    uint16_t missed_timer = 0;
    uint16_t ld2_active = 1;
    uint16_t comm_action = 0;
    
    
    while(1){
        algo_simul(); // 7ms delay
        
        if(NEW_COMM == 1){
            IEC0bits.U1RXIE = 0; // avoid char insertion in OOB position
            
            comm_action = command_recognition(COMMAND);
            COMMAND[4] = '\0';
            send_string(COMMAND); // da togliere
            COMM_INDX = 0;
            NEW_COMM = 0;
            
            // reset command string
            sprintf(COMMAND, "%s", RESET_STR);
            
            IEC0bits.U1RXIE = 1;
        }
        
        if (comm_action == 1){
            LATAbits.LATA0 = (!LATAbits.LATA0);
            comm_action = 0;
        }else if(comm_action == 2){
            switch(ld2_active){
                case 1: ld2_active = 0; break;
                case 0: ld2_active = 1; break;
            }
            comm_action = 0;
        }
        
        if(LED_BL_COUNTER == 20){
            if(ld2_active == 1){
                LATGbits.LATG9 = (!LATGbits.LATG9);
            }
            LED_BL_COUNTER = 0;
        }
        
        if (BUTTON_E8 == 1){
            //send_char('a');
            //send_string("C=", 11);
            //LATGbits.LATG9 = 1;// ----> per qualche ragione adesso questo led si 
                                // accende subito senza premere nessun tasto
            //temp = 11;
            //send_number(11);
            button_action(1, REC_CHAR);
            //LATGbits.LATG9 = (!LATGbits.LATG9);
            BUTTON_E8 = 0;
        }
        
        if (BUTTON_E9 == 1){
            //send_char('b');
            //send_string("D=", 22);
            //LATAbits.LATA0 = 1; 
            
            //temp = 22;
            //send_number(22);
            button_action(2, missed_timer);
            //LATGbits.LATG9 = (!LATGbits.LATG9);
            BUTTON_E9 = 0;
        }
    
        if(tmr_wait_period(TIMER1) == 1){
            missed_timer++;
        }
        
        // this function returns the value of the TIMER flag upon entering the call, 
        // a 0 --> dedline not reached
        // a 1 --> dedline reached (and missed)
        
        // the problem is if the flag goes up and than we wait ANOTHER 10ms before checking
        // we would loose a "missed deadline"
    } // blocking the code
    
    return 0;
}
