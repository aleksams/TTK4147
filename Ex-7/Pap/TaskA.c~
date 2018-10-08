#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>
#include <native/mutex.h>
#include <sys/mman.h>
#include <rtdk.h>
#include <sched.h>
#include <pthread.h>
#include <unistd.h>

RT_TASK task1;
RT_TASK task2;
RT_TASK taskSync;

RT_SEM semaphore_sync;
RT_MUTEX mutex_resourceA;
RT_MUTEX mutex_resourceB;


void busy_wait_us(unsigned long delay){
	for(; delay > 0; delay--){
		rt_timer_spin(1000);
	}
}


void low(void* args){
	int taskNum = *(int*)args;
	rt_printf("Task %d waiting...\n", taskNum);
	rt_sem_p(&semaphore_sync, TM_INFINITE);
	
	rt_mutex_acquire(&mutex_resourceA, TM_INFINITE);
	rt_printf("Task %d got resource A...\n", taskNum);
	busy_wait_us(300);
	rt_mutex_acquire(&mutex_resourceB, TM_INFINITE);
	rt_printf("Task %d got resource B...\n", taskNum);	
	busy_wait_us(300);

	rt_printf("Task %d dropped resource B...\n", taskNum);
	rt_mutex_release(&mutex_resourceB);
	rt_printf("Task %d dropped resource A...\n", taskNum);
	rt_mutex_release(&mutex_resourceA);
	busy_wait_us(100);
	
	rt_task_delete(NULL);
}


void high(void* args){
	int taskNum = *(int*)args;
	rt_printf("Task %d waiting...\n", taskNum);
	rt_sem_p(&semaphore_sync, TM_INFINITE);
	
	rt_task_sleep(100000);
	rt_mutex_acquire(&mutex_resourceB, TM_INFINITE);
	rt_printf("Task %d got resource B...\n", taskNum);
	busy_wait_us(100);
	rt_mutex_acquire(&mutex_resourceA, TM_INFINITE);
	rt_printf("Task %d got resource A...\n", taskNum);
	busy_wait_us(200);

	rt_printf("Task %d dropped resource A...\n", taskNum);
	rt_mutex_release(&mutex_resourceA);
	rt_printf("Task %d dropped resource B...\n", taskNum);
	rt_mutex_release(&mutex_resourceB);
	busy_wait_us(100);

	rt_task_delete(NULL);
}


void boss(void* args){
	rt_task_sleep(100000000);
	rt_sem_broadcast(&semaphore_sync);
	rt_task_sleep(100000000);
	rt_task_delete(NULL);
}


int main(){

	rt_print_auto_init(1);
	mlockall(MCL_CURRENT | MCL_FUTURE);


	// do work
	const char* streng1 = "A";
	const char* streng3 = "C";
	const char* streng4 = "Sem_sync";
	const char* streng5 = "Sem_resA";
	const char* streng6 = "Sync";
	const char* streng7 = "Sem_resB";

	int args[3] = {1, 2, 3};

	rt_sem_create( &semaphore_sync, streng4, 0, S_PRIO);
	rt_mutex_create( &mutex_resourceA, streng5);
	rt_mutex_create( &mutex_resourceB, streng7);

	rt_task_create( &task1, streng1, 0, 1, T_CPU(1) | T_JOINABLE);
	rt_task_create( &task2, streng3, 0, 3, T_CPU(1) | T_JOINABLE);
	rt_task_create( &taskSync, streng6, 0, 4, T_CPU(1) | T_JOINABLE);

	rt_task_start(&task1, &low, &args[0]);
	rt_task_start(&task2, &high, &args[1]);
	rt_task_start(&taskSync, &boss, NULL);

	//int set_periodic_v1 = rt_task_set_periodic( &task1, TM_NOW, 1000000);
	//int set_periodic_v2 = rt_task_set_periodic( &task2, TM_NOW, 1000000);
	//int set_periodic_v3 = rt_task_set_periodic( &task3, TM_NOW, 1000000);

	
  rt_task_join(&task1);
	rt_task_join(&task2);
	
	rt_sem_delete(&semaphore_sync);
	rt_mutex_delete(&mutex_resourceA);
	rt_mutex_delete(&mutex_resourceB);

	//while(1){ sleep(-1); }
	
}

