/* 
 * File:   interrupt.h
 * Author: utente
 *
 * Created on March 20, 2024, 4:42 PM
 */

#define TIMER1 1
#define TIMER2 2

void tmr_setup_period(int timer, int ms);
void tmr_wait_period(int timer);

