#ifndef MOTION_H_
#define MOTION_H_

#include "find.h"

typedef struct drivetrain {
    float wheelDia;
    float wheelCirc;
    float wheelSep;
    float gearRatio;
    float turnsPerCm;
    driveMotors drivingMotors;
}drivetrain;

extern double gyroAngle;
void *GyroAng(void *calibration_number_void);
int *toLoc(int targetLoc[], int currentLoc[], int speed);
void motion_init(int motor1Port, float wheelDiameter);
void driveStraight(int distance, float angle);
#endif // MOTION_H_
