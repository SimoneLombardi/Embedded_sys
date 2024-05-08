/*
 * File:   spi.c
 * Author: enric
 *
 * Created on 3 maggio 2024, 16.14
 */


#include "xc.h"
#include "spi.h"
#include "timer.h"




void spi_setup(){
    //setup of pin for SPI connection
    TRISAbits.TRISA1 = 1; // RA1-RPI17 MISO
    TRISFbits.TRISF12 = 0; //RF12 -RP108 clock
    TRISFbits.TRISF13 = 0; // RF13-RP109 MOSI
    TRISDbits.TRISD6=0; // chip select of magnetometer
    TRISBbits.TRISB3=0;
    TRISBbits.TRISB4=0;
    
    LATDbits.LATD6=1;
    LATBbits.LATB3=1;
    LATBbits.LATB4=1;
    
    RPINR20bits.SDI1R = 0b0010001; //mapping pin SDI1 (MISO) to pin RPI17
    RPOR12bits.RP109R = 0b000101; //mapping SDO1 (MOSI) to pin RF13
    RPOR11bits.RP108R= 0b000110; //SPI clock
    //setup of registers for SPI specs
    SPI1CON1bits.MSTEN=1;// master mode
    SPI1CON1bits.MODE16=0; //8-bit mode 
    SPI1CON1bits.PPRE= 2; // 4:1
    SPI1CON1bits.SPRE= 5; // 3:1 frequency=6MHz
    SPI1CON1bits.CKP = 1; //idle clock for some reason !!!
    SPI1STATbits.SPIEN= 1; //SPI enable    
}

void spi_magOn(){
     //magnetometer setting to sleep mode->change bit 0 of register 0x4B to "1"
    LATDbits.LATD6=0; //set pin value to low to begin communication 
    uint8_t power_control_reg= 0x4B;
    int8_t trash;
    trash=spi_write(power_control_reg);
    trash=spi_write(0x01);
    LATDbits.LATD6=1;
    tmr_wait_ms(TIMER1,2);
    
    //magnetometer setting to active mode -> change bit 1 & 2 of register 0x4C to "0" "0"
    LATDbits.LATD6=0;
    uint8_t OpMode_reg=0x4C;
    trash=spi_write(OpMode_reg);
    trash=spi_write(0x30); //correspond to 0b0011000 allows for simultaneous transition to active mode and setting of freq. tp 25 Hz
    LATDbits.LATD6=1;
    tmr_wait_ms(TIMER1,2);
}

uint16_t spi_magRead(uint16_t address, uint16_t mask, uint16_t divide){
    int16_t LSB=0;
    int16_t MSB=0;
    int16_t trash;
    CS = 0;
    trash = spi_write(address | read_mask);
    LSB = spi_write(0x00);
    MSB = spi_write(0x00);
    CS = 1;
    LSB = LSB & mask; //mask 0b00000011111000
    MSB = MSB << 8; //left-shift msb by 8
    MSB = MSB | LSB; //bit-wise OR with LSB
    MSB = MSB / divide;   
    return MSB;
}


int spi_write(unsigned int addr){
    while(SPI1STATbits.SPITBF); //waiting buffer to free up
    SPI1BUF=addr;
    while(SPI1STATbits.SPIRBF==0); //waiting buffer to receive something
    return SPI1BUF;
}
