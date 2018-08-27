#include <sched.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <x86intrin.h>

void main(){

    int ns_max = 500;
    int histogram[ns_max];
    memset(histogram, 0, sizeof(int)*ns_max);

    struct timespec t1;
    struct timespec t2;

    for(int i = 0; i < 10*1000*1000; i++){


        clock_gettime(CLOCK_MONOTONIC, &t1);
        sched_yield();
        clock_gettime(CLOCK_MONOTONIC, &t2);

        int ns = (t2.tv_nsec - t1.tv_nsec);

        if(ns >= 0 && ns < ns_max){
            histogram[ns]++;
        }
    }

    for(int i = 0; i < ns_max; i++){
        printf("%d\n", histogram[i]);
    }

}
