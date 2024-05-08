/*
 * File:   main.c
 * Author: enric
 *
 * Created on May 7, 2024, 4:37 PM
 */


#include <p33EP512MU810.h>

#include "xc.h"
//#include "uart.h"
#include "spi.h"
#include "timer.h"
#include "math.h"
#include "string.h"
#include "stdio.h"

#define mag_buff_dim 5
#define uart_buff_dim_tx 30 // circular buffer dimension
#define uart_buff_dim_rx 30

#define RX 1
#define TX 0

int16_t x_sum;
int16_t y_sum;
int16_t z_sum;
uint8_t counter;
uint16_t trash;
uint16_t LSB;
uint16_t MSB;

void __attribute__((__interrupt__, no_auto_psv__))_U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0; // reset the flag of the TX reg
}

int uart_setup(int TX_interrupt_type) {
    // UART SET UP

    RPINR18bits.U1RXR = 0x4b; // remapping the RX reg to the particular pin 
    RPOR0bits.RP64R = 1; // activating the pin of the TX reg

    // baude rate fixed to 9600 
    U1BRG = 468;

    U1MODEbits.UARTEN = 1; // enable uart COMMUNICATION
    U1STAbits.UTXEN = 1; // enable uart trasrmission 

    // TX reg INTERR type selection
    if (TX_interrupt_type < 0 || TX_interrupt_type > 3) {
        return 0;
    } else {
        switch (TX_interrupt_type) {
            case 0:U1STAbits.UTXISEL0 = 0;
                U1STAbits.UTXISEL1 = 0;
                break;
            case 1:U1STAbits.UTXISEL0 = 1;
                U1STAbits.UTXISEL1 = 0;
                break;
            case 2:U1STAbits.UTXISEL0 = 0;
                U1STAbits.UTXISEL1 = 1;
                break;
            default:break;
        }
        IEC0bits.U1TXIE = 1; // enable the TX interrupt
    }

    // RX reg INTERR, fixed on interrupt on single char recived
    U1STAbits.URXISEL0 = 0; // RX interr set to trigger for every char recived
    U1STAbits.URXISEL1 = 0;
    IEC0bits.U1RXIE = 0; // RX interr enable

    return 1;
}

void uart_send_char(char carattere) {
    while (U1STAbits.UTXBF);
    U1TXREG = carattere;
}

void uart_send_string(char *input_string) {
    char toSend_str[100];

    strcpy(toSend_str, input_string);
    for (uint16_t i = 0; i < strlen(input_string); i++) {
        uart_send_char(toSend_str[i]);
    }
}

void __attribute__((__interrupt__, __auto_psv__))_T2Interrupt(void) {
    IEC0bits.T2IE = 0;
    x_sum+=spi_magRead(0x42,0x00F8,8);
    y_sum+=spi_magRead(0x44,0x00F8,8);
    z_sum+=spi_magRead(0x46,0x00FE,2);
    counter++;
    IFS0bits.T2IF=0;
    IEC0bits.T2IE=1;
    TMR2=0;
}

void algorithm(){
    tmr_wait_ms(TIMER1,7); 
}

int main(void) {
    //setup
    uart_setup(0); //set uart interrupt to fire each time a char is trans
    spi_setup(); //set spi frequency at 6MHz
    spi_magOn(); //set magnetometer to active mode at 25 Hz
    tmr_setup_period(TIMER2, 40, 1);
    
    TRISGbits.TRISG9 = 1;
    
    
    while(1){
        LATGbits.LATG9 = 1;
        algorithm();
        if(counter==mag_buff_dim){
            counter=0;
            int16_t yaw=atan2(y_sum/mag_buff_dim,x_sum/mag_buff_dim);
            char toSend[uart_buff_dim_tx];
            sprintf(toSend,"$MAG,%d,%d,%d,$YAW,%d",x_sum/mag_buff_dim, y_sum/mag_buff_dim, z_sum/mag_buff_dim, yaw);
            uart_send_string(toSend);
            x_sum=0;y_sum=0;z_sum=0;
        }  
    }
   
    return 0;
}
