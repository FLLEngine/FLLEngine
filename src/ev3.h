#ifndef EV3_H_
#define EV3_H_

void writeFileStr(char filePath[], char contents[]);
void writeFileInt(char filePath[], int contents);
int readFileInt(char filePath[]);

#endif // EV3_H_
