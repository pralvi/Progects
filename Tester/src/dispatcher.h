#ifndef __DISPATCHER_H__
#define __DISPATCHER_H__

#include "init.h"

#ifndef TASK_LIST_SIZE
    #define TASK_LIST_SIZE 16
#endif

typedef void (*TASK_ACTION)(uint8_t tag);

uint8_t task_runOnce(TASK_ACTION task, uint32_t delay, uint8_t tag);
uint8_t task_run(TASK_ACTION task, uint16_t interval, uint8_t tag);
uint8_t task_toggle(TASK_ACTION task, uint16_t interval, uint8_t tag);
uint8_t task_reset(uint8_t id, TASK_ACTION task, uint32_t delay);
uint8_t task_cancel(uint8_t id, TASK_ACTION task);
uint8_t task_current();
uint8_t task_find(TASK_ACTION action);

void process_tasks();

#endif  /* __DISPATCHER_H__ */

