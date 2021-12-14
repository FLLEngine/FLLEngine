#include "ev3.h"

#include <stdio.h>
#include <string.h>

void writeFileStr(char filePath[], char contents[]) {
    FILE * writingFile = fopen(filePath, "w");
    fprintf(writingFile, contents);
    fclose(writingFile);
}


void writeFileInt(char filePath[], int contents) {
    FILE * writingFile = fopen(filePath, "w");
    putw(contents, writingFile);
    fclose(writingFile);
}


int readFileInt(char filePath[]) {
    FILE * readingFile = fopen(filePath, "r");
    int returnValue;
    fread(&returnValue, sizeof(returnValue), 1, readingFile);
    fclose(readingFile);
    return returnValue;
}
