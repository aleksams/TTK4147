#include <stdio.h>
#include <time.h>
#include <string.h>
#include <x86intrin.h>

struct timespec timespec_sub(struct timespec lhs, struct timespec rhs);
struct timespec timespec_normalized(time_t sec, long nsec);

void main(){
/*
    const float CLOCK_FREQ_NS = 1/(2.66);

    struct timespec now;
    struct timespec start;
    struct timespec stop;
    struct timespec latency_ts;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int i = 0; i < 10*1000*1000; i++){
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
    clock_gettime(CLOCK_MONOTONIC, &stop);

    latency_ts = timespec_sub(stop, start);
    float latency = (float)latency_ts.tv_nsec/(10*1000*1000);
    printf("%f\n", latency);*/


    int ns_max = 50;
    int histogram[ns_max];
    memset(histogram, 0, sizeof(int)*ns_max);

    struct timespec t1;
    struct timespec t2;

    for(int i = 0; i < 10*1000*1000; i++){


        clock_gettime(CLOCK_MONOTONIC, &t1);
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

struct timespec timespec_sub(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec - rhs.tv_sec, lhs.tv_nsec - rhs.tv_nsec);
}

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
