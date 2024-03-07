/*
 * File:   main_timer.c
 * Author: Family
 *
 * Created on March 7, 2024, 10:49 AM
 */


#include "xc.h"
#include "time.h"

int main(void) {
    // disable analog pin
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    
    
    return 0;
}
