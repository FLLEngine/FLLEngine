#include "ev3.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

void writeFileStr(char filePath[], char contents[]) {
    FILE * writingFile = fopen(filePath, "w");
    fprintf(writingFile, contents);
    fclose(writingFile);
}


void writeFileInt(char filePath[], int contents) {
    FILE * writingFile = fopen(filePath, "w");
    char contentsText[10];
    snprintf(contentsText, 10,"%d", contents);
    fprintf(writingFile, contentsText);
    fclose(writingFile);
}


int readFileInt(char filePath[]) {
    FILE * readingFile = fopen(filePath, "r");
    rewind(readingFile);
    int returnValue;
    fread(&returnValue, sizeof(returnValue), 1, readingFile);
    fclose(readingFile);
    return returnValue;
}

int32_t readFileInt32(char filePath[]) {
    FILE * readingFile = fopen(filePath ,"r");
    rewind(readingFile);
    int32_t returnValue;
    fscanf(readingFile, "%d", &returnValue);
    fclose(readingFile);
    return returnValue;
}
