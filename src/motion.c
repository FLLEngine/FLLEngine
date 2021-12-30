#include "motion.h"
#include "fllengine.h"
#include "ev3.h"

#include "py/runtime.h"
#include "py/objstr.h"

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

attachMotors attachmentMotors;

drivetrain robot;

short gyroValue = 0;
double gyroAngle;


int motorClamp(float value) {
    int returnValue = (int)value;
    if(abs(value) > 300) {
        returnValue = 150*(value>0 ? (1) : (-1));
    }else if(abs(value)<30){
        returnValue = 30*(value>0 ? (1) : (-1));
    }
    return returnValue;
}


void motion_init(int motor1Port, int attachment1Port, float wheelDiameter, bool invert) {
    printf("\n%d\n", invert);
    drivingMotors = findMotors(motor1Port);
    attachmentMotors = findAttachments(attachment1Port);
    robot.wheelDia = wheelDiameter;
    robot.wheelCirc = (3.14159265358979*wheelDiameter)/10;
    robot.turnsPerCm = 1/robot.wheelCirc;
    if(invert) {
        robot.motorMult = -1;
    }else{
        robot.motorMult = 1;
    }
    printf("%f, %f, %f, %f", robot.wheelDia, robot.wheelCirc, robot.turnsPerCm, robot.motorMult);
    printf("Motor 1: %s | Motor 2: %s\n", drivingMotors.Motor1.root, drivingMotors.Motor2.root);
    writeFileInt(drivingMotors.Motor1.dutyCycle, 100);
    writeFileInt(drivingMotors.Motor1.rampUp, 1000);
    writeFileInt(drivingMotors.Motor2.dutyCycle, 100);
    writeFileInt(drivingMotors.Motor2.rampUp, 1000);
    writeFileInt(attachmentMotors.attachment1.dutyCycle, 100);
    writeFileInt(attachmentMotors.attachment1.rampUp, 1000);
    writeFileInt(attachmentMotors.attachment2.dutyCycle, 100);
    writeFileInt(attachmentMotors.attachment2.rampUp, 1000);
}

void attachmentPos(int attachment, int position) {
    struct Motor attobj;
    if(attachment==1){
        attobj = attachmentMotors.attachment1;
    }else{
        attobj = attachmentMotors.attachment2;
    }


    if(position>readFileInt32(attachmentMotors.attachment1.positionRD)) {
        writeFileStr(attobj.stopAction, "hold");
        writeFileInt(attobj.speedSP, 1000);
        while(position>readFileInt32(attobj.positionRD)) {
            writeFileStr(attobj.command, "run-forever");
        }
        writeFileStr(attobj.command, "stop");
    }else{
        writeFileStr(attobj.stopAction, "hold");
        writeFileInt(attobj.speedSP, -1000);
        while(position<readFileInt32(attobj.positionRD)) {
            writeFileStr(attobj.command, "run-forever");
        }
        writeFileStr(attobj.command, "stop");
    }
}

float *toLoc(float targetLoc[], float currentLoc[], int speed) {
    float targetAngle = (atan2((targetLoc[1]*-1)-currentLoc[1], targetLoc[0]-currentLoc[0]))*57.29578;  //strange number is radians to degrees conversion
    float lineX = targetLoc[0]-currentLoc[0];
    float lineY = targetLoc[1]-currentLoc[1];
    float distance = sqrt(lineX*lineX+lineY*lineY);
    if(targetAngle<0){
        targetAngle = 360+targetAngle;
    }

    printf("from: (%f, %f) | To: (%f, %f) | Speed: %d | Reletive Angle: %f\n", currentLoc[0], currentLoc[1], targetLoc[0], targetLoc[1], speed, targetAngle);

    turnAngle(targetAngle);
    driveStraight(distance, targetAngle);

    return targetLoc;
}



void driveStraight(int distance, float angle) {
    //printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", drivingMotors.Motor1.root,drivingMotors.Motor1.command,drivingMotors.Motor1.dutyCycle,drivingMotors.Motor1.rampUp,drivingMotors.Motor1.speedSP,drivingMotors.Motor1.stopAction,drivingMotors.Motor1.speedRD,drivingMotors.Motor1.positionRD);
    float adjDistance = distance*robot.turnsPerCm*360;
    float startingDistance = (readFileInt32(drivingMotors.Motor1.positionRD)*robot.motorMult+readFileInt32(drivingMotors.Motor2.positionRD)*robot.motorMult)/2;
    writeFileStr(drivingMotors.Motor1.stopAction, "coast");
    writeFileStr(drivingMotors.Motor2.stopAction, "coast");
    while( ((double)((readFileInt32(drivingMotors.Motor1.positionRD)*robot.motorMult + readFileInt32(drivingMotors.Motor2.positionRD)*robot.motorMult)/2-startingDistance) < adjDistance && (double)((readFileInt32(drivingMotors.Motor1.positionRD)*robot.motorMult + readFileInt32(drivingMotors.Motor2.positionRD)*robot.motorMult)/2-startingDistance) > 0-adjDistance)) {  //compares average of motor positions to target distance
        usleep(10000);
        double error = fmod((float)(angle-gyroAngle+540),(360))-180;
        writeFileInt(drivingMotors.Motor1.speedSP, (-200+((error*5)))*robot.motorMult);
        writeFileInt(drivingMotors.Motor2.speedSP, (-200+(0-(error*5)))*robot.motorMult);
        writeFileStr(drivingMotors.Motor1.command, "run-forever");
        writeFileStr(drivingMotors.Motor2.command, "run-forever");
    }
    writeFileStr(drivingMotors.Motor1.stopAction, "hold");
    writeFileStr(drivingMotors.Motor2.stopAction, "hold");
    writeFileStr(drivingMotors.Motor1.command, "stop");
    writeFileStr(drivingMotors.Motor2.command, "stop");
}


void turnAngle(float angle) {
    float startingAngle=gyroAngle;
    float necesaryTurn = fmod((float)(angle-startingAngle+540),(360))-180; //finds the ideal turn to get to target angle
    float targetAngle = startingAngle+necesaryTurn;
    writeFileStr(drivingMotors.Motor1.stopAction, "coast");
    writeFileStr(drivingMotors.Motor2.stopAction, "coast");
    while(gyroAngle<targetAngle-0.5 || gyroAngle>targetAngle+0.5) {
        float error = targetAngle-gyroAngle;
        //printf("%f | %f | %f | %f \n", targetAngle, gyroAngle, error, fallback);
        int motor1Val = motorClamp(((error*3)*robot.motorMult));
        int motor2Val = motorClamp((0-(error*3)*robot.motorMult));
        writeFileInt(drivingMotors.Motor1.speedSP, motor1Val);
        writeFileInt(drivingMotors.Motor2.speedSP, motor2Val);
        writeFileStr(drivingMotors.Motor1.command, "run-forever");
        writeFileStr(drivingMotors.Motor2.command, "run-forever");
    }
    writeFileStr(drivingMotors.Motor1.stopAction, "hold");
    writeFileStr(drivingMotors.Motor2.stopAction, "hold");
    writeFileStr(drivingMotors.Motor1.command, "stop");
    writeFileStr(drivingMotors.Motor2.command, "stop");
}


void *GyroAng(void * starting_angle_void) {
    int calibration_number = 5000;
    int starting_angle;
    starting_angle = (int)starting_angle_void;
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
    gyroAngle = starting_angle;
    clock_gettime(CLOCK_REALTIME,&recent);
    while(0<1) {
        FILE* gyroValueFile = fopen(valueFileLoc, "rb");
        rewind(gyroValueFile);
        fread(&gyroValue, sizeof(gyroValue), 1, gyroValueFile);
        previous = recent;
        clock_gettime(CLOCK_REALTIME,&recent);
        gyroAngle = gyroAngle+((double)(gyroValue-normal_offset)*(((double)(recent.tv_nsec - previous.tv_nsec)/1000000000)+(recent.tv_sec - previous.tv_sec)));
        fclose(gyroValueFile);
        printf("angle: %f\r",gyroAngle);
        usleep(1000);
    }
    return NULL;
}
