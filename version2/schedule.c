#include "schedule.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

static unsigned int gettime(){
    struct timeval tv;
    if(gettimeofday(&tv, NULL) < 0){
        perror("gettimeofday");
        exit(-1);
    }
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

struct task_struct *pick(){
    int current_id = current->id;
    int i;
    
    struct task_struct *next = NULL;
    
repeat:
    for(i = 0; i <= NR_TASKS; i++){
        if(task[i] && task[i]->status == THREAD_SLEEP){
            if(gettime() > task[i]->wakeup_time){
                task[i]->status = THREAD_RUNNING;
            }
        }
    }
    
    i = current_id;
    
    while(1){
        i = (i + 1) % NR_TASKS;
        if(i == current_id){
            goto repeat;
        }
        if(task[i] && task[i]->status == THREAD_RUNNING){
            next = task[i];
            break;
        }
    }
    return next;
}

void schedule(){
    struct task_struct *next = pick();
    if(next){
        switch_to(next);
    }
}

void mysleep(int seconds){
    current->wakeup_time = gettime() + 1000 * seconds;
    current->status = THREAD_SLEEP;
    schedule();
}
