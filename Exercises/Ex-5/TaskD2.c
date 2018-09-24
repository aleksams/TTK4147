#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include "io.h"
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
// gcc main.c io.c -lpthread -lcomedi -lm -std=gnu11

/// 'struct timespec' functions ///

struct timespec timespec_normalized(time_t sec, long nsec){
    while(nsec >= 1000000000){
        nsec -= 1000000000;
        ++sec;
    }
    while(nsec < 0){
        nsec += 1000000000;
        --sec;
    }
    return (struct timespec){sec, nsec};
}

struct timespec timespec_add(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec + rhs.tv_sec, lhs.tv_nsec + rhs.tv_nsec);
}

/// Assigning CPU core ///

int set_cpu(int cpu_number){
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);

	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

// Note the argument and return types: void*
void* fn(void* args){

    // thread checks signal from BRRT and respond
    int channel = *(int*)args;
    set_cpu(1);
    
    struct timespec waketime;
    clock_gettime(CLOCK_REALTIME, &waketime);

    struct timespec period = {.tv_sec = 0, .tv_nsec = 1*1000*1000};

    while(1){
        int r_data = io_read(channel);

        if(r_data == 0){
            //printf("Signal detected");
            io_write(channel, 0);
            usleep(3);
            io_write(channel, 1);
        }
        usleep(1);
        waketime = timespec_add(waketime, period);
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
    }

}

void* disturbance(){
    set_cpu(1);
    while(1){
        asm volatile(" " ::: "memory");
    }
}

int main(){
    //pthread_t threadHandle;
    //pthread_create(&threadHandle, NULL, fn, NULL);    
    //pthread_join(threadHandle, NULL);

    int init_returns = io_init();

    pthread_t threadHandles[3];
    pthread_t distThreadHandles[10];
    int args[3] = {1, 2, 3};
    int numThreads = 3;
    
    for(int i = 0; i < numThreads; i++){
        pthread_create(&threadHandles[i], NULL, fn, &args[i]);
        printf("created thread number %d\n", i);
    }

    for(int i = 0; i < 10; i++){
        pthread_create(&distThreadHandles[i], NULL, disturbance, NULL);
    }

    for(int i = 0; i < numThreads; i++){
        pthread_join(threadHandles[i], NULL);
    }    

    for(int i = 0; i < 10; i++){
        pthread_join(distThreadHandles[i], NULL);
    }

    printf("io_init return %d\n", init_returns);
    return 0;
}