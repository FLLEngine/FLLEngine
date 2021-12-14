#include "motion.h"
#include "fllengine.h"
#include "ev3.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include "find.h"


driveMotors drivingMotors;


void motion_init(int motor1Port) {
    printf("test\n");
    drivingMotors = findMotors(motor1Port);
    printf("Motor 1: %s | Motor 2: %s\n", drivingMotors.Motor1.root, drivingMotors.Motor2.root);
    writeFileInt(drivingMotors.Motor1.dutyCycle, 100);
    writeFileInt(drivingMotors.Motor1.rampUp, 1000);
}

int *toLoc(int targetLoc[], int currentLoc[], int speed) {
    float targetAngle = (atan2(targetLoc[0]-currentLoc[0], targetLoc[1]-currentLoc[1]))*57.29578;  //strange number is radians to degrees conversion
    if(targetAngle<0){
        targetAngle = 360+targetAngle;
    }

    printf("from: (%d, %d) | To: (%d, %d) | Speed: %d | Reletive Angle: %f\n", currentLoc[0], currentLoc[1], targetLoc[0], targetLoc[1], speed, targetAngle);
    return targetLoc;
}



void driveStraight(int distance, double angle) {
    //printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", drivingMotors.Motor1.root,drivingMotors.Motor1.command,drivingMotors.Motor1.dutyCycle,drivingMotors.Motor1.rampUp,drivingMotors.Motor1.speedSP,drivingMotors.Motor1.stopAction,drivingMotors.Motor1.speedRD,drivingMotors.Motor1.positionRD);
    int startingDistance = readFileInt(drivingMotors.Motor1.positionRD);
    printf("%d\n", startingDistance);
    writeFileStr(drivingMotors.Motor1.stopAction, "coast");
    writeFileStr(drivingMotors.Motor2.stopAction, "coast");
    while((readFileInt(drivingMotors.Motor1.positionRD) + readFileInt(drivingMotors.Motor2.positionRD) - (startingDistance * 2))/2 < distance) {  //compares average of motor positions to target distance
        double error = gyroAngle - angle;
        writeFileStr(drivingMotors.Motor1.command, "stop");
        writeFileStr(drivingMotors.Motor2.command, "stop");
        writeFileInt(drivingMotors.Motor1.speedSP, (0-error));
        writeFileInt(drivingMotors.Motor2.speedSP, error);
        writeFileStr(drivingMotors.Motor1.command, "run-forever");
        writeFileStr(drivingMotors.Motor2.command, "run-forever");
    }
    writeFileStr(drivingMotors.Motor1.stopAction, "hold");
    writeFileStr(drivingMotors.Motor2.stopAction, "hold");
}
