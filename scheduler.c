#include <msp430.h>
#include <stdint.h>
#include "scheduler.h"

uint16_t registeredTasks = 0;
uint16_t *schedStackPtr;

sFIFO fifoList[MIN_NICENESS+1];

void fifoInit() {
    int i, j;
    for (i = 0; i <= MIN_NICENESS; ++i) {
        fifoList[i].head = 0;
        fifoList[i].tail = 0;
        fifoList[i].size = 0;
        for (j = 0; j < MAXTASKS; ++j) {
            fifoList[i].tasks[j] = 0;
        }
    }
}

void fifoPut(sTask *task, uint8_t niceness) {
    if (niceness < 0 || niceness > MIN_NICENESS) {
        return;
    }
    sFIFO *fifo = &fifoList[niceness];
    uint8_t nextTail = (fifo->tail+1)%MAXTASKS;
    if (nextTail == fifo->head) {
        return;
    }
    fifo->tail = nextTail;
    fifo->tasks[nextTail] = task;
}

void fifoPop(uint8_t niceness, sTask *task) {
    if (niceness < 0 || niceness > MIN_NICENESS) {
        return;
    }
    sFIFO *fifo = &fifoList[niceness];
    if (fifo->head == fifo->tail) {
        return;
    }
    task = fifo->tasks[fifo->head];
    fifo->head = (fifo->head+1)%MAXTASKS;
}

__attribute__((naked))
__attribute__((__interrupt__(WDT_VECTOR)))
void wdt_isr(){
    /*
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
    */
}

/*
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
*/
