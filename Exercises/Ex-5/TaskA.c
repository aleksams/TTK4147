#include <pthread.h>
#include <stdlib.h>
#include "io.h"
#include <stdio.h>
#include <unistd.h>
// gcc main.c io.c -lpthread -lcomedi -lm -std=gnu11

// Note the argument and return types: void*
void* fn(void* args){

    // thread checks signal from BRRT and respond
    int channel = *(int*)args;

    while(1){
        int r_data = io_read(channel);

        if(r_data == 0){
            //printf("Signal detected");
            io_write(channel, 0);
            usleep(3);
            io_write(channel, 1);
        }
        usleep(1);
    }

}

int main(){
    //pthread_t threadHandle;
    //pthread_create(&threadHandle, NULL, fn, NULL);    
    //pthread_join(threadHandle, NULL);

    int init_returns = io_init();

    pthread_t threadHandles[3];
    int args[3] = {1, 2, 3};
    int numThreads = 3;
    
    for(int i = 0; i < numThreads; i++){
        pthread_create(&threadHandles[i], NULL, fn, &args[i]);
        printf("created thread number %d\n", args[i]);
    }

    for(int i = 0; i < numThreads; i++){
        pthread_join(threadHandles[i], NULL);
    }    

    printf("io_init return %d\n", init_returns);
    return 0;
}


