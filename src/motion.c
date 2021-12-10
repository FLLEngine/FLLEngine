#include "motion.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include "find.h"


struct Motors drivingMotors;


void motion_init(int motor1Port) {
    printf("test\n");
    drivingMotors = findMotors(motor1Port);
    printf("Motor 1: %s | Motor 2: %s\n", drivingMotors.Motor1, drivingMotors.Motor2);
}

int *toLoc(int targetLoc[], int currentLoc[], int speed) {
    float targetAngle = (atan2(targetLoc[0]-currentLoc[0], targetLoc[1]-currentLoc[1]))*57.29578;  //strange number is radians to degrees conversion
    if(targetAngle<0){
        targetAngle = 360+targetAngle;
    }

    printf("from: (%d, %d) | To: (%d, %d) | Speed: %d | Reletive Angle: %f\n", currentLoc[0], currentLoc[1], targetLoc[0], targetLoc[1], speed, targetAngle);
    return targetLoc;
}



void driveStraight() {

}
