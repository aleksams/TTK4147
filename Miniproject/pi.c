
//#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#include "pi.h"

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
    
    double error            = c->reference - y;
    c->integral             += error * dt;
    c->prev_error           = error;    
    
    return c->Kp * error + c->Ki * c->integral;
}

