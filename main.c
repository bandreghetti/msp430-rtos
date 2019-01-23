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

    // registerTask(task0, 0, 1);
    // registerTask(task1, 0, 1);

    WDTCTL = WDTPW | WDTSSEL__ACLK | WDTTMSEL | WDTIS__512 | WDTCNTCL;

    // enable wdt interruptions
    SFRIFG1 &= ~WDTIFG;
    SFRIE1 |= WDTIE;

    schedulerStart();

    return 0;
}
