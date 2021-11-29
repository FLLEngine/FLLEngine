#include "py/runtime.h"
#include "py/objstr.h"


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>


const char init_line[] = "Go Mindbenders!";
pthread_t gyroThread;
double gyroAngle;
char location[35];

char *findGyro() {
    struct dirent *dir;
    DIR *rootdir = opendir("/sys/class/lego-sensor/");
    if (rootdir == NULL) {
        printf("Could not open lego-sensors directory/n");
        return 0;
    }
    printf("children of sensors directory:\n");

    while((dir = readdir(rootdir)) != NULL) {
        printf("%s\n", dir->d_name);
        if(strncmp("sensor", dir->d_name, 6)==0){
            char path[50];
            FILE *nameFile;
            char name[30];
            snprintf(path, sizeof(path), "/sys/class/lego-sensor/%s/driver_name", dir->d_name);
            nameFile=fopen(path, "r");
            fscanf(nameFile, "%s", name);
            if(strcmp(name, "lego-ev3-gyro")==0) {
                snprintf(location, sizeof(location), "/sys/class/lego-sensor/%s", dir->d_name);
                closedir(rootdir);
                return location;
            }
        }
    }
    closedir(rootdir);
    return false;
}

void *GyroAng(void *calibration_number_void) {
    int calibration_number;
    calibration_number = (int)calibration_number_void;
    printf("starting gyro\n");
    char modeFileLoc[50], valueFileLoc[50];
    findGyro();
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
    short gyroValue;
    struct timeval previous,recent,before,after;
    gettimeofday(&recent,NULL);
    double total_offset = 0;
    double normal_offset;
    gettimeofday(&before, NULL);
    for(int i=0;i<=calibration_number; i=i+1){
        FILE* gyroValueFile = fopen(valueFileLoc, "rb");
        rewind(gyroValueFile);
        fread(&gyroValue, sizeof(gyroValue), 1, gyroValueFile);
        printf("%d | %d | %f\n",i,gyroValue,total_offset);
        previous = recent;
        gettimeofday(&recent,NULL);
        total_offset = total_offset+(gyroValue*((float)(recent.tv_usec - previous.tv_usec)/1000000));
        fclose(gyroValueFile);
    }
    gettimeofday(&after,NULL);
    normal_offset = total_offset/((float)((after.tv_usec - before.tv_usec) + ((after.tv_sec - before.tv_sec)*1000000))/1000000);
    printf("%f\n",normal_offset);
    gettimeofday(&recent,NULL);
    while(true) {
        FILE* gyroValueFile = fopen(valueFileLoc, "rb");
        rewind(gyroValueFile);
        fread(&gyroValue, sizeof(gyroValue), 1, gyroValueFile);
        previous = recent;
        gettimeofday(&recent,NULL);
        gyroAngle = gyroAngle+((gyroValue-normal_offset)*((float)(recent.tv_usec - previous.tv_usec)/1000000));
        fclose(gyroValueFile);
        usleep(10);
    }
    return NULL;
}

//STATIC mp_obj_t fll_init() {
//    pthread_t gyroThread;
//}

STATIC mp_obj_t fll_start_gyro(mp_obj_t calibration_num_obj) {
    int calibration_num = mp_obj_get_int(calibration_num_obj);
    pthread_t gyroThread;
    pthread_create(&gyroThread, NULL, GyroAng, (void *) calibration_num);
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fll_start_gyro_obj, fll_start_gyro);

STATIC mp_obj_t fll_stop_gyro() {
    pthread_cancel(gyroThread);
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(fll_stop_gyro_obj, fll_stop_gyro);


STATIC mp_obj_t fll_test(mp_obj_t text_mp_obj) {
    mp_check_self(mp_obj_is_str_or_bytes(text_mp_obj));
    GET_STR_DATA_LEN(text_mp_obj, pre_text, text_len);
    char text[text_len];
    strcpy(text, (char *)pre_text);
    printf("%s: %d\n", text, text_len);
    /* struct timeval before, after; */
    /* gettimeofday(&before,NULL); */
    /* sleep(2); */
    /* gettimeofday(&after,NULL); */
    /* double time_clocks = ((after.tv_usec - before.tv_usec) + ((after.tv_sec-before.tv_sec)*1000000))/1000000; */
    /* printf("%f",time_clocks); */
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fll_test_obj, fll_test);

STATIC mp_obj_t fll_watch_gyro(mp_obj_t sample_rate_obj) {
    //int sample_rate = mp_obj_get_int(sample_rate_obj);
    //int delay = 1000/sample_rate;
    while(true){
        sleep(1);
        printf("%f\n",gyroAngle);
    }
    printf("returning\n");
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fll_watch_gyro_obj, fll_watch_gyro);


STATIC const mp_rom_map_elem_t fll_module_globals_table[] = {
{ MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_fllengine) },
{ MP_ROM_QSTR(MP_QSTR_test), MP_ROM_PTR(&fll_test_obj) },
{ MP_ROM_QSTR(MP_QSTR_watchGyro), MP_ROM_PTR(&fll_watch_gyro_obj) },
{ MP_ROM_QSTR(MP_QSTR_startGyro), MP_ROM_PTR(&fll_start_gyro_obj) },
{ MP_ROM_QSTR(MP_QSTR_stopGyro), MP_ROM_PTR(&fll_stop_gyro_obj) },
};

STATIC MP_DEFINE_CONST_DICT(fll_module_globals, fll_module_globals_table);

const mp_obj_module_t fll_engine_cmodule = {
.base = { &mp_type_module },
.globals = (mp_obj_dict_t *)&fll_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_fllengine, fll_engine_cmodule, 1);
