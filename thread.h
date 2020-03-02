#ifndef __THREAD_H__
#define __THREAD_H__

#define STACK_SIZE 1024
#define NR_TASKS 16

struct task_struct{
    int id;
    void (*th_fn)();
    int esp;
    int stack[STACK_SIZE];
};

int thread_create(int *tid, void (*start_routine)());

#endif
