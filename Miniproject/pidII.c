#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "network/udp.h"
#include "time/time.h"
#include <semaphore.h>

char recvBuf[64];
char sendBuf[64];

struct timespec startTime;
struct timespec changeTime;
struct timespec endTime;
struct timespec now;
struct timespec wakeTime;


sem_t signal_sem;
sem_t controller_sem;
sem_t reciever_sem;

void* message_reciever(void* args)
{
		UDPConn* conn = (UDPConn*)args;
		//UDPConn* conn = udpconn_new("192.168.0.1", 9999);
		struct timespec sleepTime = {.tv_sec = 0, .tv_nsec = 1000*1000};;
		while(1)
		{
				sem_wait(&reciever_sem);
				if (timespec_cmp(now, endTime) > 0)
				{
						sem_post(&controller_sem);
						sem_post(&signal_sem);
						break;
				}
				
				udpconn_send(conn, "GET");
				udpconn_receive(conn, recvBuf, sizeof(recvBuf));
				
				
				if(strcmp(recvBuf,"SIGNAL") != 0)
				{
						sem_post(&controller_sem);
				} else {
						sem_post(&signal_sem);
				}
				clock_gettime(CLOCK_REALTIME, &now);
				wakeTime = timespec_add(now, sleepTime);
				//clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &wakeTime, NULL);
				while(!(timespec_cmp(now, wakeTime) > 0))
				{
						clock_gettime(CLOCK_REALTIME, &now);
				}
				
		}
}

void* system_controller(void* args)
{
		UDPConn* conn = (UDPConn*)args;
		//UDPConn* conn = udpconn_new("192.168.0.1", 9999);
		
		float prev_error = 0;
		float integral = 0;
		float reference = 1;
		float y = 0;
		float u = 0;
		float error = 0;
		float derivative = 0;
		float dt = 0;
		
		float kp = 10;//10;
		float ki = 800;//500;
		float kd = 0;//0.001;
		
		
		struct timespec diffTime;
		struct timespec prevTime = startTime;
		
		while(1)
		{
				sem_wait(&controller_sem);
				if (timespec_cmp(now, endTime) > 0)
				{
						break;
				}
				
				strtok(recvBuf, ":");
				sscanf(strtok(NULL, ":"), "%f", &y);
				//printf("Value recv: %f\n", y);
		
				clock_gettime(CLOCK_REALTIME, &now);
				if (timespec_cmp(now, changeTime) > 0)
				{
						reference = 0;
				}
				diffTime = timespec_sub(now, prevTime);
				dt = (float)diffTime.tv_sec + ((float)diffTime.tv_nsec/1000000000);
				//printf("Diff time: %lf\n", dt);
				
				error = reference - y;
				//printf("Error: %lf\n", error);
				integral += error * dt;
				//printf("Integral: %lf\n", integral);
				derivative  = (error - prev_error) / dt;
				//printf("Derivative: %lf", derivative);
				u = kp * error + ki * integral + kd * derivative;
				//printf("%f\n", u);
				
				snprintf(sendBuf, 64, "SET:%lf", u);
				//printf("Sending: %s\n", sendBuf);
				udpconn_send(conn, sendBuf);
				
				prev_error  = error;
				prevTime = now;
		
				sem_post(&reciever_sem);
				/*clock_gettime(CLOCK_REALTIME, &now);
				wakeTime = timespec_add(now, sleepTime);
				clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &wakeTime, NULL);*/
		}
}

void* signal_responder(void* args)
{
		UDPConn* conn = (UDPConn*)args;
		//UDPConn* conn = udpconn_new("192.168.0.1", 9999);
		while(1)
		{
				sem_wait(&signal_sem);
				if (timespec_cmp(now, endTime) > 0)
				{
						break;
				}
				//printf("Got Signal!");
				udpconn_send(conn, "SIGNAL_ACK");
		
				sem_post(&reciever_sem);
				/*clock_gettime(CLOCK_REALTIME, &now);
				wakeTime = timespec_add(now, sleepTime);
				clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &wakeTime, NULL);*/
		}
}

int main()
{   
		UDPConn* conn = udpconn_new("192.168.0.1", 9999);
		
		sem_init(&signal_sem, 0, 0);
		sem_init(&controller_sem, 0, 0);
		sem_init(&reciever_sem, 0, 0);
		
		//memset(recvBuf, 0, sizeof(recvBuf));
		
		int numThreads = 3;
    pthread_t threadHandles[numThreads];
    
    pthread_create(&threadHandles[0], NULL, message_reciever, (void*)conn);
    pthread_create(&threadHandles[1], NULL, system_controller, (void*)conn);
    pthread_create(&threadHandles[2], NULL, signal_responder, (void*)conn);
		
		printf("Starting test ...\n");
    
    udpconn_send(conn, "START");
    
    struct timespec simTime = {.tv_sec = 2, .tv_nsec = 0};
    struct timespec oneSec = {.tv_sec = 1, .tv_nsec = 0};
    
    clock_gettime(CLOCK_REALTIME, &startTime);
    endTime = timespec_add(startTime, simTime);
    changeTime = timespec_add(startTime, oneSec);
    
    // START
    sem_post(&reciever_sem);
		
		// END
		int i;
    for(i = 0; i < numThreads; i++){
        pthread_join(threadHandles[i], NULL);
    }

    udpconn_send(conn, "STOP");
    udpconn_delete(conn);
    
    sem_destroy(&signal_sem);
		sem_destroy(&controller_sem);
		sem_destroy(&reciever_sem);
 
    return 0;
}
