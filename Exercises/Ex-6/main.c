
#include <native/task.h>
#include <native/timer.h>
#include <sys/mman.h>
#include <rtdk.h>
#include <sched.h>
#include <pthread.h>
#include "files/io.h"
#include <unistd.h>

RT_TASK task1;
RT_TASK task2;
RT_TASK task3; 
RT_TASK distTask[10];

// Set single CPU for pthread threads (NOT Xenomai threads!)

int set_cpu(int cpu_number){
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);

	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void fn(void* args){
	int channel = *(int*)args;

	
	unsigned long duration = 20000000000;  // 10 second timeout
	unsigned long endTime = rt_timer_read() + duration;
	while(1){
		int r_data = io_read(channel);
		if(r_data == 0){
			io_write(channel, 0);
			io_write(channel, 1);
		}
		if(rt_timer_read() > endTime){
			rt_printf("time:  %d\n", rt_timer_read());
			rt_printf("This is channel nr %d\n", channel);
			rt_printf("Time expired\n");
			rt_task_delete(NULL);
		}
		if(rt_task_yield()){
			rt_printf("Task failed to yield\n");
			rt_task_delete(NULL);
		}
		rt_task_wait_period(NULL);
	}
}


void disturbance(){
	set_cpu(1);
	while(1){
		asm volatile(" " ::: "memory");	
	}
}
// Busy-polling that won't crash your computer (hopefully)



int main(){

	rt_print_auto_init(1);
	mlockall(MCL_CURRENT | MCL_FUTURE);


	// do work
	const char* streng1 = "A";
	const char* streng2 = "B";
	const char* streng3 = "C";
	int args[3] = {1, 2, 3};
	const char* distStr[10] = {"D", "D", "D", "D", "D", "D", "D", "D", "D", "D"};

	int create_v1 = rt_task_create( &task1, streng1, 0, 1, T_CPU(1) | T_JOINABLE);
	int create_v2 = rt_task_create( &task2, streng2, 0, 1, T_CPU(1) | T_JOINABLE);
	int create_v3 = rt_task_create( &task3, streng3, 0, 1, T_CPU(1) | T_JOINABLE);

	int start_v1 = rt_task_start(&task1, &fn, &args[0]);
	int start_v2 = rt_task_start(&task2, &fn, &args[1]);
	int start_v3 = rt_task_start(&task3, &fn, &args[2]);

	/*for(int i = 0; i < 10; i++){
		rt_task_create( &distTask[i], distStr[i], 0, 0, T_CPU(1));
		rt_task_start(	&distTask[i], &disturbance, NULL);
	}*/

	int set_periodic_v1 = rt_task_set_periodic( &task1, TM_NOW, 1000000);
	int set_periodic_v2 = rt_task_set_periodic( &task2, TM_NOW, 1000000);
	int set_periodic_v3 = rt_task_set_periodic( &task3, TM_NOW, 1000000);

	
    	int terminate_v1 = rt_task_join(&task1);
	int terminate_v2 = rt_task_join(&task2);
	int terminate_v3 = rt_task_join(&task3);
	
	/*for(int i = 0; i < 10; i++){
		rt_task_join(&distTask[i]);
	}*/
	while(1){ sleep(-1); }
	
}
