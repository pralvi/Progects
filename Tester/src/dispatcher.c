#include "dispatcher.h"
#include "atomic.h"

#define TASK_EMPTY      0x0
#define TASK_WAIT       0x1
#define TASK_DONE       0x2
#define TASK_LOCK       0x4
#define TASK_PIN        0x8

typedef struct {
    TASK_ACTION action;
    uint32_t    run_time;
    uint16_t    run_interval;
    uint8_t     status;
    uint8_t     tag;
} TASK;

static TASK task_list[TASK_LIST_SIZE];

uint8_t task_find(TASK_ACTION action) {
  for(uint_fast8_t i = 0; i < TASK_LIST_SIZE; i++) {
    uint8_t cur_status = task_list[i].status;
    if (task_list[i].action == action && cur_status != TASK_EMPTY && cur_status != TASK_DONE)
        return i + 1;
  }
  return 0;
}

uint8_t task_insert(TASK_ACTION action, uint32_t interval, uint8_t status, uint8_t tag) {
  for(uint_fast8_t i = 0; i < TASK_LIST_SIZE; i++) {
    uint8_t cur_status = task_list[i].status;
    if (cur_status == TASK_EMPTY || cur_status == TASK_DONE)
        if (CompareExchange8((uint8_t*)&task_list[i].status, cur_status, TASK_LOCK)) {
            task_list[i].action = action;
            task_list[i].run_interval = interval;
            task_list[i].run_time = GetSysTick() + interval;
            task_list[i].tag = tag;
            task_list[i].status = status;
            return i + 1;
        }
  }
  return 0;
}

uint8_t task_runOnce(TASK_ACTION task, uint32_t delay, uint8_t tag) {
    return task_insert(task, delay, TASK_WAIT, tag);
}

uint8_t task_run(TASK_ACTION task, uint16_t interval, uint8_t tag) {
    return task_insert(task, interval, TASK_PIN | TASK_WAIT, tag);
}

uint8_t task_toggle(TASK_ACTION task, uint16_t interval, uint8_t tag) {
    uint8_t id = task_find(task);
    if (id) {
        task_cancel(id, task);
        return 0;
    }

    return task_run(task, interval, tag);
}

uint8_t task_reset(uint8_t id, TASK_ACTION task, uint32_t delay) {
  if (id > 0 && id <= TASK_LIST_SIZE && task_list[id-1].action == task) {
      TASK* t = &task_list[id-1];
      uint8_t status = t->status;
      if (((status & TASK_WAIT) == TASK_WAIT) && CompareExchange8((uint8_t*)&t->status, status, TASK_LOCK)) {
          if (delay)
            t->run_interval = delay;
          t->run_time = GetSysTick() + t->run_interval;
          t->status = status;
          return 1;
      }
  }
  return 0;
}

uint8_t task_cancel(uint8_t id, TASK_ACTION task) {
  if (id > 0 && id <= TASK_LIST_SIZE && task_list[id-1].action == task) {
      uint8_t status = task_list[id-1].status;
      if ((status != TASK_LOCK) && CompareExchange8((uint8_t*)&task_list[id-1].status, status, TASK_DONE))
          return 1;
  }

  return 0;
}

uint_fast8_t currentTask = 0;
uint8_t task_current() {
    return currentTask + 1;
}

void process_tasks_for_period(uint32_t prev_time, uint32_t cur_time) {
    for(currentTask = 0; currentTask < TASK_LIST_SIZE; currentTask++) {
      uint8_t status = task_list[currentTask].status;
      if ((status & TASK_WAIT) && task_list[currentTask].run_time >= prev_time && task_list[currentTask].run_time <= cur_time) {
            TASK_ACTION action = task_list[currentTask].action;
            uint8_t tag = task_list[currentTask].tag;
            if ((status & TASK_PIN) ? task_reset(currentTask + 1, action, 0) : CompareExchange8((uint8_t*)&task_list[currentTask].status, status, TASK_DONE))
                action(tag);

      }
    }
}

void process_tasks() {
    static uint32_t prev_time = 0;
    uint32_t tick = GetSysTick();

    if (prev_time > tick) { //turnover
        process_tasks_for_period(prev_time, 0xFFFFFFFF);
        prev_time = 0;
    }

    process_tasks_for_period(prev_time, tick);
    prev_time = tick;
}
