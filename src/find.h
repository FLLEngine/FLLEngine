#ifndef FIND_H
#define FIND_H

char *findGyro();

struct Motors findMotors(int motor1Port);


struct Motors {
    char Motor1[40];
    char Motor2[40];
};

#endif // FIND_H
