#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

#include "thread.h"

extern struct task_struct *task[NR_TASKS];
extern struct task_struct *current;

void switch_to(struct task_struct *next);

struct task_struct *pick();
void schedule();
void mysleep(int seconds);

#endif
