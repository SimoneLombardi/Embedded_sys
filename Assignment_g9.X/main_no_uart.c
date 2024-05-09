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
#define uart_buff_dim_tx 35 // circular buffer dimension
// aumentato a 35 per tenere conto dei segni '-', effetto: nessun effetto apparente 23.08
#define uart_buff_dim_rx 30

#define RX 1
#define TX 0

int16_t x_sum;
int16_t y_sum;
int16_t z_sum;
// PROBLEMA : variabile somme mai inizializzate
    // effetto inizializzazione globale: nessun effetto apparente 23.14
    // effetto inizializzazione nel main: nessun effetto apparente 23.16

uint8_t counter;
uint16_t trash;
int16_t LSB; //potrebbero creare conflitto --> tolti entrambi si è ciulata anche la x, z rimane ok
int16_t MSB; // avvicinando il telefono la x si sistema, seconda prova valori paiono casuali

// reinserite le definizioni come dei int16_t --> x, y ciulate; z ok

// PROBLEMA : variabile counter mai inizializzata
    // effetto inizializzazione globale: nessun effetto apparente 22.50
    // effetto inizializzazione nel main: nessun effetto apparente 22.55

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
    x_sum = x_sum + spi_magRead(0x42,0x00F8,8);
    y_sum = y_sum + spi_magRead(0x44,0x00F8,8);
    z_sum = z_sum + spi_magRead(0x46,0x00FE,2);
    // utilizzo della sintassi estesa, effetto: nessun effetto apparente 23.10
    
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
    
    counter = 0;
    
    x_sum = 0;
    y_sum = 0;
    z_sum = 0;
    
    int16_t x_avg;
    int16_t y_avg;
    int16_t z_avg;
    
    tmr_setup_period(TIMER2, 40, 1);
    
    TRISGbits.TRISG9 = 0; // set led to output

    while(1){
        if(SPI1STATbits.SPIROV == 0){
            LATGbits.LATG9 = 1;
        }                    // notata la non accensione del led 22.53
                            // notata la mia idiozia --> led settato come input 23.33
                            // controllo con led bit SPIROV : nessuna accensione 23.36
                            // cambio con SPIROV == 0: led acceso (valori di x, y ancora fottuti)
        algorithm();
        if(counter == mag_buff_dim){
            counter=0;
            
            x_avg = x_sum/mag_buff_dim;
            y_avg = y_sum/mag_buff_dim;
            z_avg = z_sum/mag_buff_dim;
            
            double yaw = atan2((float)y_avg,(float)x_avg);
            char toSend[uart_buff_dim_tx];
            // nota: divisione intera restituisce intero OK, mag_buff_dim non ha un TIPO in senso stretto
            //          viene sostituito con '5' a run time
            sprintf(toSend,"$MAG,%d,%d,%d,$YAW,%.2f",x_avg, y_avg, z_avg, yaw);
            // riduzione del carico di dati da inviare, effetto: nessun effetto apparente 23.04 (reinserisco il valore di yaw)
            uart_send_string(toSend);
            x_sum=0;y_sum=0;z_sum=0;
        }  
    }
   
    return 0;
}

// modifica definizione della funz-> spi_write, aggiunta variabile locale per il ritorno
    // effetto: nessun effetto apparente 00.01

// PROBLEMONE: funzione magRead ritorna un uint16_t modifica a valore di ritorno a int16_t
    // modificato anche i tipi degli argomenti tutti come int16_t
    // effetto: LESSSSSGOOOOOOOOOOOO valori in uscita sensati yeeeeee 00.17

// modifica: funizione atan2 richiede valori float come argomenti

