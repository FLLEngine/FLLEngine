#ifndef MOTION_H_
#define MOTION_H_

#include "find.h"

int *toLoc(int targetLoc[], int currentLoc[], int speed);
void motion_init(int motor1Port);

#endif // MOTION_H_
