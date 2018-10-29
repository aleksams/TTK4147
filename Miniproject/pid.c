#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "network/udp.h"
#include "time/time.h"

float prev_error = 0;
float integral = 0;
float reference = 1.0;

int kp = 10;
int ki = 800;
int kd = 1;


void* reciever(void* args)
{
    int conn = *(UDPConn*)args;
		char recvBuf[64];

    udpconn_receive(conn, recvBuf, sizeof(recvBuf));
    printf("Recieved: %s\n", recvBuf);
    if(recvBuf[0] == "G"){
			strtok(recvBuf, ":");
			sscanf(strtok(NULL, ":"), "%lf", &server_value);
    }else{
    	
    }
}

void* transmitter(void* args)
{
    udpconn_send(conn, "GET");
    snprintf(sendBuf, 64, "SET:%lf", ret_value);
		printf("Sent: %s\n", sendBuf);
		udpconn_send(conn, sendBuf);

}

void* controller(void* args)
{
    float error = reference - y;
		integral += error * dt;
		float derivative  = (error - prev_error) / dt;
		prev_error  = error;
    new_value = kp * error + ki * integral + kd * derivative;
}


int main()
{   
		printf("Starting test ...\n");
    UDPConn* conn = udpconn_new("192.168.0.1", 9999);
    
    
    char sendBuf[64];
    float server_value;   
    float ret_value; 
    memset(recvBuf, 0, sizeof(recvBuf));

    udpconn_send(conn, "START");
    
    pthread_t threadHandles[3];
    
    pthread_create(&threadHandles[0], NULL, reciever, &conn);
    pthread_create(&threadHandles[1], NULL, transmitter, &conn);
    pthread_create(&threadHandles[2], NULL, controller, NULL);

    for(int i = 0; i < numThreads; i++){
        pthread_join(threadHandles[i], NULL);
    }

    udpconn_send(conn, "STOP");
    udpconn_delete(conn);
 
    return 0;
}
