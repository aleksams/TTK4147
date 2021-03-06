#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>
#include <sys/mman.h>
#include <rtdk.h>
#include <sched.h>
#include <pthread.h>
#include <unistd.h>

RT_TASK task1;
RT_TASK task2;
RT_TASK task3;

RT_SEM semaphore;

void fn(void* args){
	int taskNum = *(int*)args;
	rt_printf("Task %d waiting...\n", taskNum);
	rt_sem_p(&semaphore, TM_INFINITE);
	rt_printf("Task %d done\n", taskNum);
	rt_task_delete(NULL);
}


void boss(void* args){
	int taskNum = *(int*)args;
	rt_task_sleep(100000000);
	rt_sem_broadcast(&semaphore);
	rt_task_sleep(100000000);
	rt_sem_delete(&semaphore);
	rt_printf("Task %d deleted semaphore\n", taskNum);
	rt_task_delete(NULL);
}


int main(){

	rt_print_auto_init(1);
	mlockall(MCL_CURRENT | MCL_FUTURE);


	// do work
	const char* streng1 = "A";
	const char* streng2 = "B";
	const char* streng3 = "C";
	const char* streng4 = "Sem";
	int args[3] = {1, 2, 3};

	rt_sem_create( &semaphore, streng4, 0, S_PRIO);
	

	rt_task_create( &task1, streng1, 0, 2, T_CPU(1) | T_JOINABLE);
	rt_task_create( &task2, streng2, 0, 1, T_CPU(1) | T_JOINABLE);
	rt_task_create( &task3, streng3, 0, 3, T_CPU(1) | T_JOINABLE);

	rt_task_start(&task1, &fn, &args[0]);
	rt_task_start(&task2, &fn, &args[1]);
	rt_task_start(&task3, &boss, &args[2]);

	//int set_periodic_v1 = rt_task_set_periodic( &task1, TM_NOW, 1000000);
	//int set_periodic_v2 = rt_task_set_periodic( &task2, TM_NOW, 1000000);
	//int set_periodic_v3 = rt_task_set_periodic( &task3, TM_NOW, 1000000);

	
  rt_task_join(&task1);
	rt_task_join(&task2);
	rt_task_join(&task3);
	
	//while(1){ sleep(-1); }
	
}

