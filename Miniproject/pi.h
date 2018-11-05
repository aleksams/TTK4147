//#pragma once
#include "time/time.h"


typedef struct PIController PIcontroller;

// Allocates a new generic-ish PI controller
PIcontroller* pi_new(double Kp, double Ki);

// Sets the reference point of the controller
void pi_setReference(PIcontroller* c, double ref);

// Generates a control input (u) given a new measurement (y)
// Periodic execution is preferable but not necessary, since
//  the PID_controller stores the time it was last updated
// For this reason, this function assumes that the time between 
//  two successive calls is less than one second
double pi_actuate(PIcontroller* c, double y);
