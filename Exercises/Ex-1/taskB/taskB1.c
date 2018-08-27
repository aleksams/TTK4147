#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <x86intrin.h>


void main(){

    const float CLOCK_FREQ_NS = 1/(2.66);

    /*
    int start = __rdtsc();
    for(int i = 0; i < 10*1000*1000; i++){
        int tsc = __rdtsc();
    }
    int stop = __rdtsc();
    double latency = (stop - start) / 2.66 / (10*1000*1000); //(10*1000*1000) * CLOCK_FREQ_NS;
    printf("%f\n", latency);
    */

    int ns_max = 50;
    int histogram[ns_max];
    memset(histogram, 0, sizeof(int)*ns_max);

    for(int i = 0; i < 10*1000*1000; i++){

        int t1 = __rdtsc();
        int t2 = __rdtsc();

        int ns = (t2 - t1) * (CLOCK_FREQ_NS);

        if(ns >= 0 && ns < ns_max){
            histogram[ns]++;
        }
    }

    for(int i = 0; i < ns_max; i++){
        printf("%d\n", histogram[i]);
    }

}
