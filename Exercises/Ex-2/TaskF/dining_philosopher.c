#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

pthread_mutex_t forks[5];
//pthread_mutex_t eating;
sem_t eating;

void* philosopher(void* a){
    int test = *(int*) a;
    while(1){
        //printf("philosopher, %d\n", test);
        //pthread_mutex_lock(&eating);
        sem_wait(&eating);
        if (test==5){
            pthread_mutex_lock(&forks[test-1]);
            //sleep(1);
            pthread_mutex_lock(&forks[0]);
        } else {
            pthread_mutex_lock(&forks[test-1]);
            //sleep(1);
            pthread_mutex_lock(&forks[test]);
        }
        sem_post(&eating);

        // got forks, eating...
        printf("philosopher %d eating\n", test);
        //sleep(1);


        if (test==5){
            pthread_mutex_unlock(&forks[test-1]);
            pthread_mutex_unlock(&forks[0]);
        } else {
            pthread_mutex_unlock(&forks[test-1]);
            pthread_mutex_unlock(&forks[test]);
        }
        //pthread_mutex_unlock(&eating);
    }
}

int main(){
    //pthread_mutex_init(&eating, NULL);
    sem_init(&eating, 0, 4);
    // 2nd arg is a pthread_mutexattr_t
    for (int i = 0; i<5; i++){
        pthread_mutex_init(&forks[i], NULL);
    }

    pthread_t t1, t2, t3, t4, t5;
    int ids[] = {1, 2, 3, 4, 5};

    pthread_create(&t1, NULL, philosopher, &ids[0]);
    pthread_create(&t2, NULL, philosopher, &ids[1]);
    pthread_create(&t3, NULL, philosopher, &ids[2]);
    pthread_create(&t4, NULL, philosopher, &ids[3]);
    pthread_create(&t5, NULL, philosopher, &ids[4]);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    pthread_join(t5, NULL);

    for (int i = 0; i<5; i++){
        pthread_mutex_destroy(&forks[i]);
    }
}
