#include <msp430.h>
#include <stdint.h>
#include "clock.h"
#include "scheduler.h"

void task0() {
    P1DIR |= BIT0;
    while (1) {
        int i = 50000;
        while (--i)
            ;
        P1OUT ^= BIT0;
    }
}

void task1() {
    P4DIR |= BIT7;
    while (1) {
        int i = 25000;
        while (--i)
            ;
        P4OUT ^= BIT7;
    }
}

/**
 * main.c
 */
int main(void) {
    // set wdt as interval timer
    WDTCTL = WDTPW | WDTHOLD;

    clockInit();

    WDTCTL = WDTPW | WDTSSEL__ACLK | WDTTMSEL | WDTIS__512 | WDTCNTCL;

    // enable wdt interruptions
    SFRIFG1 &= ~WDTIFG;
    SFRIE1 |= WDTIE;

    /*
    // save scheduler stack pointer
    asm("movx.a SP, %0" : "+m" (schedStackPtr));

    asm("movx.a %0, SP" :: "m" (tasks[currentTask].stackPtr));
    asm("popm.a #12, R15");
    asm("reti");
    */

    return 0;
}
