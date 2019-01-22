#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdint.h>

#define MAXTASKS 10
#define MIN_NICENESS 2

typedef struct {
    void (*entryPoint)();
    uint16_t *stackPtr;
} sTask;

typedef struct {
    sTask *tasks[MAXTASKS];
    uint8_t head, tail;
    uint8_t size;
} sFIFO;

void fifoInit();
void fifoPut(sTask *task, uint8_t niceness);
void fifoPop(uint8_t niceness, sTask *task);

#endif /* SCHEDULER_H_ */
