#ifndef FIND_H
#define FIND_H

char *findGyro();


typedef struct Motor{
    char root[45];
    char command[45];
    char dutyCycle[45];
    char rampUp[45];
    char speedSP[45];
    char stopAction[45];
    char speedRD[45];
    char positionRD[45];
}Motor;


typedef struct driveMotors{
    struct Motor Motor1;
    struct Motor Motor2;
}driveMotors;

typedef struct attachMotors{
    struct Motor attachment1;
    struct Motor attachment2;
}attachMotors;


driveMotors findMotors(int motor1Port);
attachMotors findAttachments(int attachment1Port);
void fillMotor(char fileName[], Motor * asMotor);

#endif // FIND_H
