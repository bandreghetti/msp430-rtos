#include "scheduler.h"
#include <msp430.h>
#include <stdint.h>

uint16_t registeredTasks = 0;
uint16_t *schedStackPtr;
sTask runningTask;
uint8_t nextPID = 0;

sFIFO fifoList[MAX_NICENESS + 1];

void fifoInit() {
    int i;
    for (i = 0; i <= MAX_NICENESS; ++i) {
        fifoList[i].head = 0;
        fifoList[i].tail = 0;
        fifoList[i].size = 0;
    }
}

void fifoPut(sTask task, uint8_t niceness) {
    if (niceness < 0 || niceness > MAX_NICENESS) {
        return;
    }
    sFIFO *fifo = &fifoList[niceness];
    uint8_t nextTail = (fifo->tail + 1) % MAXFIFOSIZE;
    if (nextTail == fifo->head) {
        return;
    }
    fifo->tail = nextTail;
    fifo->tasks[nextTail] = task;
    fifo->size += 1;
}

void fifoPop(uint8_t niceness, sTask *task) {
    if (niceness < 0 || niceness > MAX_NICENESS) {
        return;
    }
    sFIFO *fifo = &fifoList[niceness];
    if (fifo->head == fifo->tail) {
        return;
    }
    *task = fifo->tasks[fifo->head];
    fifo->head = (fifo->head + 1) % MAXFIFOSIZE;
    fifo->size -= 1;
}

void exitTask() { runningTask.finished = 1; }

void idleTask() { __low_power_mode_0(); }

void schedulerStart() {
    registerTask(idleTask, MAX_NICENESS, 1);

    uint8_t n = 0;
    for (n = 0; n < MAX_NICENESS; ++n) {
        if (fifoList[n].size > 0) {
            fifoPop(n, &runningTask);
        }
    }

    schedStackPtr = 0x3200;

    asm("movx.a %0, SP" ::"m"(runningTask.stackPtr));
    asm("popm.a #12, R15");
    asm("reti");
}

void scheduler() {
    static uint8_t quantum;
    if (quantum-- > 0) return;

    fifoPut(runningTask, runningTask.niceness);

    uint8_t n = 0;
    for (n = 0; n < MAX_NICENESS; ++n) {
        if (fifoList[n].size > 0) {
            fifoPop(n, &runningTask);
        }
    }

    quantum = runningTask.quantum;

    return;
}

__attribute__((naked)) __attribute__((__interrupt__(WDT_VECTOR))) void
wdt_isr() {
    asm("pushm.a #12, R15");
    asm("movx.a SP, %0" : "+m"(runningTask.stackPtr));

    // restore scheduler stack pointer
    asm("movx.a %0, SP" ::"m"(schedStackPtr));

    scheduler();

    // save scheduler stack pointer
    asm("movx.a SP, %0" : "+m"(schedStackPtr));

    asm("movx.a %0, SP" ::"m"(runningTask.stackPtr));
    asm("popm.a #12, R15");
    asm("reti");
}

void registerTask(void (*pTask)(), uint8_t niceness, uint8_t quantum) {
    if (niceness < 0 || niceness > MAX_NICENESS) {
        return;
    }
    sFIFO *fifo = &fifoList[niceness];
    if (fifo->size >= MAXFIFOSIZE) {
        return;
    }
    sTask newTask;
    newTask.entryPoint = pTask;
    newTask.quantum = quantum;
    newTask.pid = nextPID++;
    newTask.finished = 0;
    newTask.niceness = niceness;

    newTask.stackPtr =
        (uint16_t *)(0x3280 + (fifo->size) * 0x80 + niceness * 0x80);
    *(--newTask.stackPtr) = (uint16_t *)pTask;
    *(--newTask.stackPtr) = GIE | (((uint32_t)pTask >> 4) & 0xF000);
    int i;
    for (i = 0; i < 24; ++i) {
        *(--newTask.stackPtr) = 0;
    }
    fifoPut(newTask, niceness);
}
