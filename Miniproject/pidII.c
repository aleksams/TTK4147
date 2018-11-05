#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "network/udp.h"
#include <semaphore.h>
#include "pi.h"

char recvBuf[64];
char sendBuf[64];

struct timespec now;
struct timespec startTime;
struct timespec changeTime;
struct timespec endTime;

int changeFlag = 0;
int endFlag = 0;

sem_t signal_sem;
sem_t controller_sem;
sem_t reciever_sem;

struct PIController {
    double reference;
    
    double Kp;
    double Ki;
    
    struct timespec lastUpdated;    
    
    double integral;
    double prev_error;
};


PIcontroller* pi_new(double Kp, double Ki){ 
    PIcontroller* c = malloc(sizeof(*c));    
    memset(c, 0, sizeof(*c));
    
    c->Kp = Kp;
    c->Ki = Ki;
        
    clock_gettime(CLOCK_REALTIME, &c->lastUpdated);
    
    return c;
}

void pi_setReference(PIcontroller* c, double ref){
    c->reference = ref;
}

double pi_actuate(PIcontroller* c, double y){
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    struct timespec t = timespec_sub(now, c->lastUpdated);
    double dt = t.tv_sec + t.tv_nsec / 1000000000.0;
    c->lastUpdated = now;
    //printf("Diff time: %lf\n", dt);
    
    double error            = c->reference - y;
    c->integral             += error * dt;
    c->prev_error           = error;    
    
    return c->Kp * error + c->Ki * c->integral;
}

void* message_reciever(void* args)
{
		UDPConn* conn = (UDPConn*)args;

		while(1)
		{
				sem_wait(&reciever_sem);
				//printf("Reciever in action\n");
				/*if (timespec_cmp(now, endTime) > 0)
				{
						sem_post(&controller_sem);
						sem_post(&signal_sem);
						break;
				}*/
				if (endFlag == 1)
				{
						sem_post(&controller_sem);
						sem_post(&signal_sem);
						break;
				}
				
				//printf("HEI");
				udpconn_send(conn, "GET");
				udpconn_receive(conn, recvBuf, sizeof(recvBuf));
				
				
				if(strcmp(recvBuf,"SIGNAL") != 0)
				{
						sem_post(&controller_sem);
						
				} else {
						sem_post(&signal_sem);
				}
				
		}
}

void* system_controller(void* args)
{
		UDPConn* conn = (UDPConn*)args;
		
		//float dt = 0;
		
		float y = 0;
		float u = 0;
		
		PIcontroller* controller = pi_new(10, 300);
		
		pi_setReference(controller, 0);
		
		
		while(1)
		{
				//udpconn_send(conn, "GET");
				sem_wait(&controller_sem);
				//printf("Controller begins\n");
				if (endFlag == 1)
				{
						sem_post(&reciever_sem);
						sem_post(&signal_sem);
						break;
				} else if(changeFlag == 1)
				{
				        pi_setReference(controller, 1);
				}
				
				strtok(recvBuf, ":");
				sscanf(strtok(NULL, ":"), "%f", &y);
				//printf("Value recv: %f\n", y);
				
				u = pi_actuate(controller, y);
		
				clock_gettime(CLOCK_REALTIME, &now);
				/*if (timespec_cmp(now, changeTime) > 0)
				{
						pi_setReference(controller, 0);
				}*/
				
				
				sprintf(sendBuf, "SET:%3.3f", u);
				udpconn_send(conn, sendBuf);
		
				sem_post(&reciever_sem);
		}
}

void* signal_responder(void* args)
{
		UDPConn* conn = (UDPConn*)args;
		
		while(1)
		{
		        //udpconn_send(conn, "GET");
				sem_wait(&signal_sem);
				//printf("Signal mottat\n");
				if (endFlag == 1)
				{
						sem_post(&reciever_sem);
						sem_post(&controller_sem);
						break;
				}
				udpconn_send(conn, "SIGNAL_ACK");
		
				sem_post(&reciever_sem);
		}
}

int main()
{   
	UDPConn* conn = udpconn_new("192.168.0.1", 9999);
		
	sem_init(&signal_sem, 0, 0);
	sem_init(&controller_sem, 0, 0);
	sem_init(&reciever_sem, 0, 0);
		
	memset(recvBuf, 0, sizeof(recvBuf));
		
	int numThreads = 3;
    pthread_t threadHandles[numThreads];
    
    pthread_create(&threadHandles[0], NULL, message_reciever, (void*)conn);
    pthread_create(&threadHandles[1], NULL, system_controller, (void*)conn);
    pthread_create(&threadHandles[2], NULL, signal_responder, (void*)conn);
		
	//printf("Starting test ...\n");
    
    udpconn_send(conn, "START");
    
    struct timespec simTime = {.tv_sec = 2, .tv_nsec = 0};

    struct timespec oneSec = {.tv_sec = 1, .tv_nsec = 0};
    
    clock_gettime(CLOCK_REALTIME, &startTime);
    
    endTime = timespec_add(startTime, simTime);
    changeTime = timespec_add(startTime, oneSec);
    
    // START
    sem_post(&reciever_sem);
    
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &changeTime, NULL);
    changeFlag = 1;
    
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &endTime, NULL);
    endFlag = 1;
		
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

