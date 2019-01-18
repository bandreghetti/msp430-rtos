#include <msp430.h>
#include <stdint.h>
#include "clock.h"

typedef struct {
    void (*entryPoint)();
    uint16_t *stackPtr;
} sTask;

sTask tasks[10];
uint16_t registeredTasks = 0;
int currentTask = 0;
uint16_t *schedStackPtr;

#define MAXTASKS 10

void registerTask(void (*pTask)()) {
    if (registeredTasks < MAXTASKS) {
        sTask task;
        task.entryPoint = pTask;
        task.stackPtr = (uint16_t*)(0x2800 + registeredTasks*0x80);
        *(--task.stackPtr) = (uint16_t*)pTask;
        *(--task.stackPtr) = GIE | (((uint32_t)pTask>>4) & 0xF000);
        int i;
        for (i = 0; i < 24; ++i) {
            *(--task.stackPtr) = 0;
        }
        tasks[registeredTasks++] = task;
    }
}

void task0(){
    P1DIR |= BIT0;
    while(1) {
        int i = 50000;
        while(--i);
        P1OUT ^= BIT0;
    }
}

void task1(){
    P4DIR |= BIT7;
    while(1) {
        int i = 25000;
        while(--i);
        P4OUT ^= BIT7;
    }
}

/**
 * main.c
 */
int main(void)
{
    registerTask(task0);
    registerTask(task1);

    // set wdt as interval timer
    WDTCTL = WDTPW | WDTHOLD;

    clockInit();

    WDTCTL = WDTPW | WDTSSEL__ACLK | WDTTMSEL | WDTIS__512 | WDTCNTCL;

    // enable wdt interruptions
    SFRIFG1 &= ~WDTIFG;
    SFRIE1 |= WDTIE;

    // save scheduler stack pointer
    asm("movx.a SP, %0" : "+m" (schedStackPtr));

    asm("movx.a %0, SP" :: "m" (tasks[currentTask].stackPtr));
    asm("popm.a #12, R15");
    asm("reti");

    return 0;
}

__attribute__((naked))
__attribute__((__interrupt__(WDT_VECTOR)))
void wdt_isr(){
    asm("pushm.a #12, R15");
    asm("movx.a SP, %0" : "+m" (tasks[currentTask].stackPtr));

    // restore scheduler stack pointer
    asm("movx.a %0, SP" :: "m" (schedStackPtr));

    //scheduler
    if (++currentTask >= registeredTasks) {
        currentTask = 0;
    }

    // save scheduler stack pointer
    asm("movx.a SP, %0" : "+m" (schedStackPtr));

    asm("movx.a %0, SP" :: "m" (tasks[currentTask].stackPtr));
    asm("popm.a #12, R15");
    asm("reti");
}
