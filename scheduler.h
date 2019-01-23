#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdint.h>

#define MAXFIFOSIZE 4
#define MAX_NICENESS 2

typedef struct {
    void (*entryPoint)();
    uint16_t *stackPtr;
    uint8_t quantum;
    uint8_t finished;
    uint8_t niceness;
    uint8_t pid;
} sTask;

typedef struct {
    sTask tasks[MAXFIFOSIZE];
    uint8_t head, tail;
    uint8_t size;
} sFIFO;

void fifoInit();
void fifoPut(sTask task, uint8_t niceness);
void fifoPop(uint8_t niceness, sTask *task);
void registerTask(void (*pTask)(), uint8_t niceness, uint8_t quantum);
void schedulerStart();
void exitTask();

#endif /* SCHEDULER_H_ */
