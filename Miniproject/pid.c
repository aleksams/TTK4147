#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "network/udp.h"
#include "time/time.h"
#include <semaphore.h>

float prev_error = 0;
float integral = 0;
float reference = 1.0;
float server_value;

int kp = 10;
int ki = 800;
int kd = 1;

sem_t calc_semaphore;
sem_t send_semaphore;

void* reciever(void* args)
{
		//UDPConn* conn = udpconn_new("192.168.0.1", 9999);
    UDPConn* conn = (UDPConn*)args;
		char recvBuf[64];
		float temp_val;
		memset(recvBuf, 0, sizeof(recvBuf));
		
		int i;
		for(i = 0; i < 10; i++){
			printf("\n");
			udpconn_send(conn, "GET");
		  udpconn_receive(conn, recvBuf, sizeof(recvBuf));
		  printf("Recieved: %s\n", recvBuf);
		  
		  if(strcmp(recvBuf,"SIGNAL") != 0){
		  	printf("plain receive buffer: %s\n", recvBuf);
		  	
				strtok(recvBuf, ":");
				sscanf(strtok(NULL, ":"), "%s %f", &temp_val); // recvBuf is the 4 last digits of server_value after
				printf("Server value at receiver %lf:\n", temp_val);
				server_value = temp_val;
				printf("posting calc_sem\n");
				sem_post(&calc_semaphore);
				
		  } else {
		    udpconn_send(conn, "SIGNAL_ACK");
		    printf("SIGNAL_ACK sendt \n");
		  }
		}
}

void* transmitter(void* args)
{
		//UDPConn* conn = udpconn_new("192.168.0.1", 9999);
		UDPConn* conn = (UDPConn*)args;
		char sendBuf[64];
		float ret_value = 1.0;
		
		int i;
		for(i = 0; i < 10; i++){
			sem_wait(&send_semaphore);
		  snprintf(sendBuf, 64, "SET:%lf", ret_value);
			udpconn_send(conn, sendBuf);
			//printf("Sent: %s\n", sendBuf);
		  //udpconn_send(conn, "GET");
		}
}

void* controller(void* args)
{
		int i;
		for(i = 0; i < 10; i++){
			//printf("Server value %lf:\n", server_value);
			sem_wait(&calc_semaphore);
			printf("Controller starting ...\n");
			printf("Server value at controller %lf:\n", server_value);
			float y = 0;
			float dt = 1;
		  float error = reference - y;
			integral += error * dt;
			float derivative  = (error - prev_error) / dt;
			prev_error  = error;
		  float new_value = kp * error + ki * integral + kd * derivative;
		}
}


int main()
{   
		UDPConn* conn = udpconn_new("192.168.0.1", 9999);
		sem_init(&calc_semaphore, 0, 0); 
		sem_init(&send_semaphore, 0, 0); 
		
		printf("Starting test ...\n");
    
    udpconn_send(conn, "START");
    
    int numThreads = 3;
    pthread_t threadHandles[numThreads];
    
    pthread_create(&threadHandles[0], NULL, reciever, (void*)conn);
    pthread_create(&threadHandles[1], NULL, transmitter, (void*)conn);
    pthread_create(&threadHandles[2], NULL, controller, NULL);
		
		int i;
    for(i = 0; i < numThreads; i++){
        pthread_join(threadHandles[i], NULL);
    }

    udpconn_send(conn, "STOP");
    udpconn_delete(conn);
 
    return 0;
}

