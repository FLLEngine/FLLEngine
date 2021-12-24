#ifndef EV3_H_
#define EV3_H_

#include <stdint.h>

void writeFileStr(char filePath[], char contents[]);
void writeFileInt(char filePath[], int contents);
int readFileInt(char filePath[]);
int32_t readFileInt32(char filePath[]);

#endif // EV3_H_
