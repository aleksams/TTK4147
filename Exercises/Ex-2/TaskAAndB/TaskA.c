
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#include <unistd.h>

long aleks = 0;

void* thread1(){
    long eiv = 0;
    for(int i  = 0; i < 50*1000*1000; i++){
        eiv = eiv + 1;
        aleks = aleks + 1;
        eiv = eiv - 1;
        aleks = aleks - 1;
    }
    printf("eiv1: %ld\n", eiv);
    printf("aleks1: %ld\n", aleks);
}

void* thread2(){
    long eiv = 0;
    for(int i  = 0; i < 50*1000*1000; i++){
        eiv = eiv + 1;
        aleks = aleks + 1;
        eiv = eiv - 1;
        aleks = aleks - 1;
    }
    printf("eiv2: %ld\n", eiv);
    printf("aleks2: %ld\n", aleks);
}

int main(){
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread1, NULL);
    pthread_create(&t2, NULL, thread2, NULL);
    //sleep(2);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}
