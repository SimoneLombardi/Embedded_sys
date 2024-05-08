/*
 * File:   uart.c
 * Author: Family
 *
 * Created on April 29, 2024, 7:59 AM
 */


#include "xc.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"


char buffer_rx[uart_buff_dim_rx];

uint16_t uart_elem_cnt_rx;
uint16_t uart_tail_rx;
uint16_t uart_head_rx;

char buffer_tx[uart_buff_dim_tx];

uint16_t uart_elem_cnt_tx;
uint16_t uart_tail_tx;
uint16_t uart_head_tx;

uint16_t new_char;

void __attribute__((__interrupt__, no_auto_psv__))_U1RXInterrupt(void) {
    IFS0bits.U1RXIF = 0; // reset the flag of the RX reg

    new_char = 1;
    uart_buff_add(RX);
}

void __attribute__((__interrupt__, no_auto_psv__))_U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0; // reset the flag of the TX reg


}

// possible TX_interrupt_type
//0: interrupt when any char is transfered to the Transmit Shift Register
//1: interrupt when the last character is removed from Transmit Shift Register
//2: interrupt when the last char is transferred to the Transmit Shift register
//3: RESERVED

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

int uart_is_buff_full(int buff) {
    switch (buff) {
        case RX:if (uart_elem_cnt_rx == uart_buff_dim_rx) {
                return 1;
            }
            break;
        case TX:if (uart_elem_cnt_tx == uart_buff_dim_tx) {
                return 1;
            }
            break;
    }
    return 0;
}

int uart_is_buff_empty(int buff) {
    switch (buff) {
        case RX:if (uart_elem_cnt_rx == 0) {
                return 1;
            }
            break;
        case TX:if (uart_elem_cnt_tx == 0) {
                return 1;
            }
            break;
    }
    return 0;
}

int uart_buff_add(int buff) {
    switch (buff) {
        case RX:if (uart_is_buff_full(RX)) {
                return 0; // buffer_rx is full, can't add any elements
                // ADD SOME WAY TO DEBUG THIS ERROR
                }
                // save the char recived
                buffer_rx[uart_tail_rx] = U1RXREG;

                // update the index of the buffer_rx
                uart_elem_cnt_rx = uart_elem_cnt_rx + 1; // element counter
                uart_tail_rx = (uart_tail_rx + 1) % (uart_buff_dim_rx);// tail index
                break;
        case TX:if (uart_is_buff_full(TX)) {
                return 0; // buffer_rx is full, can't add any elements
                // ADD SOME WAY TO DEBUG THIS ERROR
                }
                // TO ADD ---> funzione di salvataggio valori del MAG
                

                // update the index of the buffer_rx
                uart_elem_cnt_rx = uart_elem_cnt_rx + 1; // element counter
                uart_tail_rx = (uart_tail_rx + 1) % (uart_buff_dim_rx);// tail index
                break;
    }

    return 1; // successfull operation return
}

int uart_buff_rmv(int buff) {
    if (uart_is_buff_empty(uart_elem_cnt_rx)) {
        return 0; // buffer_rx is empty, can't remove any elements
    }

    // update the index of the buffer_rx
    uart_elem_cnt_rx = uart_elem_cnt_rx - 1; // element counter
    uart_head_rx = (uart_head_rx + 1) % (uart_buff_dim_rx); // head index

    return 1; // successfull operation return
}

void uart_send_head(int buff) {
    // print the char in the head position
    while (U1STAbits.UTXBF);
    U1TXREG = buffer_rx[uart_head_rx];
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


/*
void uart_log_buffer_info(){
    char toSend[100];
    // numero_elementi_nel_buffer_rx, indice_testa, indice_coda
    sprintf(toSend, "%d,%d,%d", uart_elem_cnt_rx, uart_head_rx, uart_tail_rx);
    uart_send_string(toSend);
}*/
