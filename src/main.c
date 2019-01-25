#include <msp430.h>
#include <stdint.h>
#include "clock.h"
#include "scheduler.h"

void task0() {
    P1DIR |= BIT0;
    int n = 100;
    while (n--){
        int i = 25000;
        while (--i);
        P1OUT ^= BIT0;
    }
    exitTask();
}

void task1() {
    P4DIR |= BIT7;
    int n = 200;
    while (n--) {
        int i = 50000;
        while (--i);
        P4OUT ^= BIT7;
    }
    exitTask();
}
 void task2(){
     P2REN = BIT1;
     P1DIR |= BIT0;
     while(1){
         if ((P2IN & BIT1)!=BIT1){
             P1OUT |= BIT0;
             int i = 25000;
             while (--i);
         }
     }
 }

 void task3(){
     P4DIR |= BIT7;
     P1REN = BIT1;
     while(1){
         if ((P1IN & BIT1)!=BIT1){
             P4OUT |= BIT7;
             int i = 25000;
             while (--i);
         }
     }
 }
/**
 * main.c
 */
int main(void) {

    // set wdt as interval timer
    WDTCTL = WDTPW | WDTHOLD;

    clockInit();

    fifoInit();
    registerTask(task0, 0, 1);
    registerTask(task1, 0, 10);

    WDTCTL = WDTPW | WDTSSEL__ACLK | WDTTMSEL | WDTIS__512 | WDTCNTCL;

    // enable wdt interruptions
    SFRIFG1 &= ~WDTIFG;
    SFRIE1 |= WDTIE;

    schedulerStart();

    return 0;
}
