#include "find.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>


char outPorts[][16] = {"ev3-ports:outA", "ev3-ports:outB", "ev3-ports:outC", "ev3-ports:outD"};



// returns the sensor directory with the gyro most recently connected to the robot
char *findGyro() {
    char *location = malloc(sizeof(char) * 35);
    printf("\n\nfinding gyro\n\n");
    struct dirent *dir;
    DIR *rootdir = opendir("/sys/class/lego-sensor/");
    if (rootdir == NULL) {
        printf("Could not open lego-sensors directory/n");
        return 0;
    }
    printf("__children of sensors directory________________\n|                                             |\n");

    while((dir = readdir(rootdir)) != NULL) {
        printf("|                                             |\r");
        printf("| %s\n", dir->d_name);
        if(strncmp("sensor", dir->d_name, 6)==0){
            char path[50];
            FILE *nameFile;
            char name[30];
            snprintf(path, sizeof(path), "/sys/class/lego-sensor/%s/driver_name", dir->d_name);
            nameFile=fopen(path, "r");
            fscanf(nameFile, "%s", name);
            if(strcmp(name, "lego-ev3-gyro")==0) {
                snprintf(location, 35, "/sys/class/lego-sensor/%s", dir->d_name);
                closedir(rootdir);
                printf("|_____________________________________________|\n");
                return location;
            }
        }
    }
    printf("|_____________________________________________|\n");
    closedir(rootdir);
    return false;
}



driveMotors findMotors(int motor1Port) {
    driveMotors foundMotors;
    printf("\n\nfinding motors\n\n");
    struct dirent *dir;
    DIR *rootdir = opendir("/sys/class/tacho-motor/");
    if (rootdir == NULL) {
        printf("Could not open tacho-motor directory/n");
        return foundMotors;
    }
    printf("__children of motors directory_________________\n|                                             |\n");

    while((dir = readdir(rootdir)) != NULL) {
        printf("|                                             |\r");
        printf("| %s\n", dir->d_name);
        if(strncmp("motor", dir->d_name, 5)==0){
            char path[50];
            char addressPath[50];
            FILE *nameFile;
            FILE *addressFile;
            char address[30];
            char name[30];
            snprintf(path, sizeof(path), "/sys/class/tacho-motor/%s/driver_name", dir->d_name);
            nameFile=fopen(path, "r");
            fscanf(nameFile, "%s", name);
            fclose(nameFile);
            if(strcmp(name, "lego-ev3-l-motor")==0) {
                snprintf(addressPath, sizeof(addressPath), "/sys/class/tacho-motor/%s/address", dir->d_name);
                addressFile=fopen(addressPath, "r");
                fscanf(addressFile, "%s", address);
                if(strcmp(address, outPorts[motor1Port])==0){
                    fillMotor(dir->d_name, &foundMotors.Motor1);
                }else{
                    fillMotor(dir->d_name, &foundMotors.Motor2);
                }
            }
        }
    }
    closedir(rootdir);
    printf("|_____________________________________________|\n");
    return foundMotors;
}

void fillMotor(char fileName[], Motor * asMotor){
    snprintf(asMotor->root, 45, "/sys/class/tacho-motor/%s/", fileName);
    snprintf(asMotor->command, 45, "/sys/class/tacho-motor/%s/command", fileName);
    snprintf(asMotor->dutyCycle, 45, "/sys/class/tacho-motor/%s/duty_cycle_sp", fileName);
    snprintf(asMotor->rampUp, 45, "/sys/class/tacho-motor/%s/ramp_up_sp", fileName);
    snprintf(asMotor->speedSP, 45, "/sys/class/tacho-motor/%s/speed_sp", fileName);
    snprintf(asMotor->stopAction, 45, "/sys/class/tacho-motor/%s/stop_action", fileName);
    snprintf(asMotor->speedRD, 45, "/sys/class/tacho-motor/%s/speed", fileName);
    snprintf(asMotor->positionRD, 45, "/sys/class/tacho-motor/%s/position", fileName);

}
