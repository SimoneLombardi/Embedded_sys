/*
 * File:   main_uart_def.c
 * Author: Family
 *
 * Created on April 16, 2024, 12:10 PM
 */


#include "xc.h"
#include "timer.h"
#include "stdio.h"
#include "string.h"

// selection variable for INTERR driven event
uint16_t EVENT_NO = 0; // default event = 0
// INT1 interrupt = 1
// INT2 interrupt = 2
// T1   interrupt = 3
// RX   interrupt = 4

// command string 
char COMMAND[4]; // string containig the command recived
char RESET_STRING[4] = {'\0'};
uint16_t pos = 0; // command string index

// ---------------------------------- interrupts function ---------------------------------- //

void __attribute__((__interrupt__, no_auto_psv__)) _INT1Interrupt(void){
    IEC1bits.INT1IE = 0; // ENABLE to zero
    IEC1bits.INT2IE = 0; // avoid other button press and bouncing problem
    
    // when a button press is detected we block BOTH button, is in line whith 
    // the assignement to avoid the conflict of both button using the uart
    
    IFS1bits.INT1IF = 0; // reset the flag of the corresponding button
    
    EVENT_NO = 1; // set event no variable
}

void __attribute__((__interrupt__, no_auto_psv__)) _INT2Interrupt(void){
    IEC1bits.INT1IE = 0; // ENABLE to zero
    IEC1bits.INT2IE = 0;
    
    IFS1bits.INT2IF = 0; // reset the flag of the corresponding button
    
    EVENT_NO = 2;
}

void __attribute__((__interrupt__, no_auto_psv__)) _T1Interrupt(){
    IFS0bits.T1IF = 0; // reset flag timer
    //IEC0bits.T1IE = 0; // reset enable timer
    
    EVENT_NO = 3;
    //tpf_led_blink = tpf_led_blink + 1; //to make LD2 blink at 2.5Hz
    // first method only 1 enable is restored in each statements
}

void __attribute__((__interrupt__, no_auto_psv__))_U1RXInterrupt(void){
    IFS0bits.U1RXIF = 0; // reset INTERR flag to 0
    EVENT_NO = 4; 
    
    // save char recived
    COMMAND[pos] = U1RXREG; // save the new char in the current pos
    pos++; // increase pos for the nex char
    
    if (pos == 4){ // reset pos if the end of string is reached
        pos = 0;
    }
}

void __attribute__((__interrupt__, no_auto_psv__))_U1TXInterrupt(void){
    IFS0bits.U1TXIF = 0; // reset the flag of the TX reg
    
    // since the TX reg is now empty we can restore the button
    IEC1bits.INT1IE = 1;
    IEC1bits.INT2IE = 1;
}

// ---------------------------------- utility function ---------------------------------- //
void send_char(char carattere){
    U1TXREG = carattere;
}

void send_string(char *stringa){
    char toSend_string[100];
    
    sprintf(toSend_string, "%s", stringa);
    
    for (uint16_t i = 0; i < strlen(send_string); i++){
        send_char(toSend_string[i]);
    }
}

void simul_algo(uint16_t time, uint16_t timer){
    tmr_wait_ms(timer, time);
}

// ---------------------------------- exercise function ---------------------------------- //
void button_action(uint16_t valore){
    char toSend_string[100];
    
    switch(EVENT_NO){
        case 1: sprintf(toSend_string, "C=%d", valore);break;
        case 2: sprintf(toSend_string, "D=%d", valore);break;
        default: break;
    }
    
    send_string(toSend_string);
}

void command_recognition(char *command_string){
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
    U1STAbits.UTXISEL0 = 1; 
    U1STAbits.UTXISEL1 = 0; 
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
    
    // variable definition
    uint16_t recivedChar_count = 0;
    uint16_t missedDline_conut = 0;
    uint16_t timer2Cycle_conut = 0;
    
    uint16_t led2_active = 1;
    uint16_t recived_command = 0;
    
    while(1){
        simul_algo(7, TIMER2);
        
        
        switch(EVENT_NO){
            case 1: button_action(recivedChar_count); EVENT_NO = 0; break;
            case 2: button_action(missedDline_conut); EVENT_NO = 0; break;
            case 3: timer2Cycle_conut++;
                    if (timer2Cycle_conut == 20){
                        if (led2_active == 1){
                            LATGbits.LATG9 = (!LATGbits.LATG9);
                        }
                        timer2Cycle_conut = 0;
                    }
                    EVENT_NO = 0;
                    break; // from case 3;
            case 4: recivedChar_count++;
                    if(strlen(COMMAND) == 4){
                        IEC0bits.U1RXIE = 0; // deactivate the char reception enab to avoid the insertion of new char in 
                                             // a out of bound position in COMMAND[4]!!!
                        recived_command = command_recognition(COMMAND);
                        switch(recived_command){
                            case 1: LATAbits.LATA0 = (!LATAbits.LATA0); recived_command = 0; break;
                            case 2: if(led2_active == 1){led2_active = 0;}else{led2_active = 1;} recived_command = 0; break;
                            default: break;
                        }
                        // reset the COMMAND string 
                        sprintf(COMMAND, "%s", RESET_STRING); // trash all of the COMMAND string content
                        pos = 0; // reset COMMAND string position index
                        IEC0bits.U1RXIE = 1; // re enable reception of the char
                    }
                    EVENT_NO = 0;
                    break; // from case 4
            default: break;
        }
        
        if(tmr_wait_period(TIMER1) == 1){ // if the deadline is missed increase the counter
            missedDline_conut++;
        }
    }
}
