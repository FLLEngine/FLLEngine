#ifndef MOTION_H_
#define MOTION_H_

#include "find.h"
#include "stdbool.h"

typedef struct drivetrain {
    float wheelDia;
    float wheelCirc;
    float wheelSep;
    float gearRatio;
    float turnsPerCm;
    float motorMult;
    driveMotors drivingMotors;
}drivetrain;

extern double gyroAngle;
void *GyroAng(void *calibration_number_void);
int *toLoc(int targetLoc[], int currentLoc[], int speed);
void motion_init(int motor1Port, float wheelDiameter, bool invert);
void driveStraight(int distance, float angle);
void turnAngle(float angle);
#endif // MOTION_H_
