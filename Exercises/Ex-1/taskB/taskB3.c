#include <stdio.h>
#include <time.h>
#include <string.h>
#include <x86intrin.h>
#include <sys/times.h>

void main(){

/*
    struct tms buf;
    clock_t start;
    clock_t stop;
    clock_t latency;
    clock_t now;

    start = times(&buf);

    for(int i = 0; i < 10*1000*1000; i++){
        now = times(&buf);
    }
    stop = times(&buf);

    latency = (stop-start);
    printf("%ld\n", latency);*/


    int ns_max = 50;
    int histogram[ns_max];
    memset(histogram, 0, sizeof(int)*ns_max);

    struct tms buf;
    clock_t t1;
    clock_t t2;

    for(int i = 0; i < 10*1000*1000; i++){
        t1 = times(&buf);
        t2 = times(&buf);

        int ns = (t2 - t1);

        if(ns >= 0 && ns < ns_max){
            histogram[ns]++;
        }
    }

    for(int i = 0; i < ns_max; i++){
        printf("%d\n", histogram[i]);
    }

}
