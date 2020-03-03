#include "schedule.h"
#include <signal.h>
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
    int i, next, cnt;
    for(i = 0; i< NR_TASKS; ++i){
        if(task[i] && task[i]->status != THREAD_EXIT && gettime() > task[i]->wakeup_time){
            task[i]->status = THREAD_RUNNING;
        }
    }
    
    while(1){
        cnt = -1;
        next = 0;
        for(i = 0; i < NR_TASKS; ++i){
            if(!task[i]){
                continue;
            }
            if(task[i] && task[i]->status == THREAD_RUNNING && task[i]->counter > cnt){
                cnt = task[i]->counter;
                next = i;
            }
        }
        
        if(cnt){
            break;
        }
        
        if(cnt == 0){
            for(i = 0; i< NR_TASKS; ++i){
                if(task[i]){
                    task[i]->counter = (task[i]->counter >> 1) + task[i]->priority;
                }
            }
        }
    }
    return task[next];
}

void openalarm(){
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    if(sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0){
        perror("sigprocmask block");
    }
}

void closealarm(){
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    if(sigprocmask(SIG_BLOCK, &mask, NULL) < 0){
        perror("sigprocmask unblock");
    }
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

static void do_timer(){
    if(--current->counter > 0){
        return;
    }
    current->counter = 0;
    schedule();
}

__attribute__((constructor))
static void init(){
    struct itimerval value;
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = 1000; //the time to start timer after run the programmer
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = 1000 * 10;
    if(setitimer(ITIMER_REAL, &value, NULL) < 0) {
        perror("setitimer");
    }
    signal(SIGALRM, do_timer);
}
