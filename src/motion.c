#include "motion.h"
#include "fllengine.h"
#include "ev3.h"

#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <stdbool.h>
#include "find.h"

//=====PID Params (WIP)=====================================
#define PID_KP  4.0f
#define PID_KI  0.5f
#define PID_KD  0.25f

#define PID_TAU 0.02f

#define PID_LIM_MIN -10.0f
#define PID_LIM_MAX  10.0f

#define PID_LIM_MIN_INT -5.0f
#define PID_LIM_MAX_INT  5.0f

#define SAMPLE_TIME_S 0.01f
//==========================================================


driveMotors drivingMotors;

drivetrain robot;

short gyroValue = 0;
double gyroAngle;


void motion_init(int motor1Port, float wheelDiameter) {
    drivingMotors = findMotors(motor1Port);
    robot.wheelDia = wheelDiameter;
    robot.wheelCirc = (3.14159265358979*((wheelDiameter/2)*(wheelDiameter/2)))/10;
    robot.turnsPerCm = 1/robot.wheelCirc;
    printf("Motor 1: %s | Motor 2: %s\n", drivingMotors.Motor1.root, drivingMotors.Motor2.root);
    writeFileInt(drivingMotors.Motor1.dutyCycle, 100);
    writeFileInt(drivingMotors.Motor1.rampUp, 1000);
    writeFileInt(drivingMotors.Motor2.dutyCycle, 100);
    writeFileInt(drivingMotors.Motor2.rampUp, 1000);
}

int *toLoc(int targetLoc[], int currentLoc[], int speed) {
    float targetAngle = (atan2(targetLoc[0]-currentLoc[0], targetLoc[1]-currentLoc[1]))*57.29578;  //strange number is radians to degrees conversion
    if(targetAngle<0){
        targetAngle = 360+targetAngle;
    }

    printf("from: (%d, %d) | To: (%d, %d) | Speed: %d | Reletive Angle: %f\n", currentLoc[0], currentLoc[1], targetLoc[0], targetLoc[1], speed, targetAngle);
    return targetLoc;
}



void driveStraight(int distance, float angle) {
    //printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", drivingMotors.Motor1.root,drivingMotors.Motor1.command,drivingMotors.Motor1.dutyCycle,drivingMotors.Motor1.rampUp,drivingMotors.Motor1.speedSP,drivingMotors.Motor1.stopAction,drivingMotors.Motor1.speedRD,drivingMotors.Motor1.positionRD);
    float adjDistance = distance*robot.turnsPerCm*360;
    int startingDistance = readFileInt(drivingMotors.Motor1.positionRD);
    printf("%d\n", startingDistance);
    writeFileStr(drivingMotors.Motor1.stopAction, "coast");
    writeFileStr(drivingMotors.Motor2.stopAction, "coast");
    printf("coast\n");
    printf("%d\n", ((readFileInt(drivingMotors.Motor1.positionRD) + readFileInt(drivingMotors.Motor2.positionRD) - (startingDistance * 2))/2) < distance);
    while( 0 < adjDistance ) {  //compares average of motor positions to target distance
        printf("%f\n", gyroAngle);
        usleep(10000);
        double error = angle-gyroAngle;
        printf("%f, %f, %f\n", error, angle, gyroAngle);
        writeFileInt(drivingMotors.Motor1.speedSP, -200+(0-(error*3)));
        writeFileInt(drivingMotors.Motor2.speedSP, -200+(error*3));
        writeFileStr(drivingMotors.Motor1.command, "run-forever");
        writeFileStr(drivingMotors.Motor2.command, "run-forever");
        printf("run-forever\n");
    }
    writeFileStr(drivingMotors.Motor1.stopAction, "hold");
    writeFileStr(drivingMotors.Motor2.stopAction, "hold");
    writeFileStr(drivingMotors.Motor1.command, "stop");
    writeFileStr(drivingMotors.Motor2.command, "stop");
    printf("hold\n");
}


void *GyroAng(void *calibration_number_void) {
    int calibration_number;
    calibration_number = (int)calibration_number_void;
    char modeFileLoc[50], valueFileLoc[50];
    char location[35];
    strcpy(location, findGyro());
    if(strncmp("/sys/class/lego-sensor/sensor",location,29)==0){
        printf("gyro found at: %s\n", location);
    }else{
        printf("no gyro found :( ... is it plugged in? (exiting)\n");
        return 0;
    }
    snprintf(modeFileLoc, sizeof(modeFileLoc), "%s/mode", location);
    snprintf(valueFileLoc, sizeof(valueFileLoc), "%s/bin_data", location);
    FILE *gyroModeFile = fopen(modeFileLoc,"w");
    fprintf(gyroModeFile, "GYRO-RATE");
    fclose(gyroModeFile);
    struct timespec previous,recent,before,after;
    clock_gettime(CLOCK_REALTIME,&recent);
    clock_gettime(CLOCK_REALTIME,&previous);
    double total_offset = 0;
    double normal_offset;
    printf("\n\nCalibrating:\n\n|sample\t|speed\t|offset\t\t|time\t\t|\n+-------+-------+---------------+---------------+\n");
    clock_gettime(CLOCK_REALTIME,&before);
    for(int i=0;i<=calibration_number; i=i+1){
        FILE* gyroValueFile = fopen(valueFileLoc, "rb");
        rewind(gyroValueFile);
        fread(&gyroValue, sizeof(gyroValue), 1, gyroValueFile);
        printf("|%d\t| %d \t| %f \t| %f\t|\r",i,gyroValue,total_offset, (((double)(recent.tv_nsec - previous.tv_nsec)/1000000000)+(recent.tv_sec - previous.tv_sec)));
        previous = recent;
        clock_gettime(CLOCK_REALTIME,&recent);
        total_offset = total_offset+(gyroValue*(((double)(recent.tv_nsec - previous.tv_nsec)/1000000000)+(recent.tv_sec - previous.tv_sec)));
        fclose(gyroValueFile);
    }
    printf("\n");
    clock_gettime(CLOCK_REALTIME,&after);
    normal_offset = total_offset/(((double)(after.tv_nsec - before.tv_nsec)/1000000000)+(after.tv_sec - before.tv_sec));
    printf("%f\n",normal_offset);
    clock_gettime(CLOCK_REALTIME,&recent);
    while(0<1) {
        FILE* gyroValueFile = fopen(valueFileLoc, "rb");
        rewind(gyroValueFile);
        fread(&gyroValue, sizeof(gyroValue), 1, gyroValueFile);
        previous = recent;
        clock_gettime(CLOCK_REALTIME,&recent);
        gyroAngle = gyroAngle+((double)(gyroValue-normal_offset)*(((double)(recent.tv_nsec - previous.tv_nsec)/1000000000)+(recent.tv_sec - previous.tv_sec)));
        fclose(gyroValueFile);
    }
    return NULL;
}
