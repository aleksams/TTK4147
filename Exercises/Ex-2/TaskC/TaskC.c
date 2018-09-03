
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

long aleks = 0;
sem_t mutex;

void* thread1(){
    long eiv = 0;
    for(int i  = 0; i < 50*1000*1000; i++){
        eiv = eiv + 1;
        eiv = eiv - 1;

        sem_wait(&mutex);

        aleks = aleks + 1;
        aleks = aleks - 1;

        sem_post(&mutex);
    }
    printf("eiv1: %ld\naleks1: %ld\n", eiv, aleks);
}

void* thread2(){
    long eiv = 0;
    for(int i  = 0; i < 50*1000*1000; i++){
        eiv = eiv + 1;
        eiv = eiv - 1;

        sem_wait(&mutex);

        aleks = aleks + 1;
        aleks = aleks - 1;

        sem_post(&mutex);
    }
    printf("eiv2: %ld\naleks2: %ld\n", eiv, aleks);
}

int main(){
    sem_init(&mutex, 0, 1);
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread1, NULL);
    pthread_create(&t2, NULL, thread2, NULL);
    //sleep(2);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    sem_destroy(&mutex);
}
