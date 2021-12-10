#include "py/runtime.h"
#include "py/objstr.h"

#include "find.h"
#include "motion.h"


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>


const char init_line[] = "Go Mindbenders!";
pthread_t gyroThread;
double gyroAngle;
short gyroValue = 0;




// calls findGyro(), calibrates using data collected over a loop whose length is controlled with argument 0, starts and infinite loop calculating the current angle of the gyro and writing it to gyroAngle
void *GyroAng(void *calibration_number_void) {
    printf("starting gyro\n");
    int calibration_number;
    calibration_number = (int)calibration_number_void;
    printf("starting gyro\n");
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
    double total_offset = 0;
    double normal_offset;
    clock_gettime(CLOCK_REALTIME,&before);
    for(int i=0;i<=calibration_number; i=i+1){
        FILE* gyroValueFile = fopen(valueFileLoc, "rb");
        rewind(gyroValueFile);
        fread(&gyroValue, sizeof(gyroValue), 1, gyroValueFile);
        printf("%d | %d | %f\n",i,gyroValue,total_offset);
        previous = recent;
        clock_gettime(CLOCK_REALTIME,&recent);
        total_offset = total_offset+(gyroValue*(((double)(recent.tv_nsec - previous.tv_nsec)/1000000000)+(recent.tv_sec - previous.tv_sec)));
        fclose(gyroValueFile);
    }
    clock_gettime(CLOCK_REALTIME,&after);
    normal_offset = total_offset/(((double)(after.tv_nsec - before.tv_nsec)/1000000000)+(after.tv_sec - before.tv_sec));
    printf("%f\n",normal_offset);
    clock_gettime(CLOCK_REALTIME,&recent);
    while(true) {
        FILE* gyroValueFile = fopen(valueFileLoc, "rb");
        rewind(gyroValueFile);
        fread(&gyroValue, sizeof(gyroValue), 1, gyroValueFile);
        previous = recent;
        clock_gettime(CLOCK_REALTIME,&recent);
        gyroAngle = gyroAngle+(gyroValue*(((double)(recent.tv_nsec - previous.tv_nsec)/1000000000)+(recent.tv_sec - previous.tv_sec)));
        fclose(gyroValueFile);
    }
    return NULL;
}

void mp_array_to_array(mp_obj_tuple_t **mp_array, int ** arr) {
    * arr = malloc(2 * sizeof(int));
    for(int i=0; i<2;i=i+1) {
        (*arr)[i] = mp_obj_get_int((*mp_array)->items[i]);
    }
}



//
STATIC mp_obj_t fll_to_loc(mp_obj_t currentLocObj, mp_obj_t targetLocObj, mp_obj_t speedObj) {
    int speed = mp_obj_get_int(speedObj);

    mp_obj_tuple_t *currentLocTObj = MP_OBJ_TO_PTR(currentLocObj);
    mp_obj_tuple_t *targetLocTObj = MP_OBJ_TO_PTR(targetLocObj);
    int *currentLoc;
    int *targetLoc;
    mp_array_to_array(&currentLocTObj, &currentLoc);
    mp_array_to_array(&targetLocTObj, &targetLoc);
    toLoc(currentLoc, targetLoc, speed);
    return targetLocObj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(fll_to_loc_obj, fll_to_loc);



STATIC mp_obj_t fll_init(mp_obj_t motor_1_obj) {
    int motor1Port = mp_obj_get_int(motor_1_obj);
    motion_init(motor1Port);
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fll_init_obj, fll_init);



// creates a thread that runs the GyroAng() function, argument 0 is passed through to argument 0 of GyroAng()
STATIC mp_obj_t fll_start_gyro(mp_obj_t calibration_num_obj) {
    int calibration_num = mp_obj_get_int(calibration_num_obj);
    pthread_t gyroThread;
    pthread_create(&gyroThread, NULL, GyroAng, (void *) calibration_num);
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fll_start_gyro_obj, fll_start_gyro);



// terminates the thread running GyroAng()
STATIC mp_obj_t fll_stop_gyro() {
    pthread_cancel(gyroThread);
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(fll_stop_gyro_obj, fll_stop_gyro);



// prints argument 0 and its length
STATIC mp_obj_t fll_test(mp_obj_t text_mp_obj) {
    mp_check_self(mp_obj_is_str_or_bytes(text_mp_obj));
    GET_STR_DATA_LEN(text_mp_obj, pre_text, text_len);
    char text[text_len];
    strcpy(text, (char *)pre_text);
    printf("%s: %d\n", text, text_len);
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fll_test_obj, fll_test);



// enters an infinite loop that prints the current angle of the gyro
STATIC mp_obj_t fll_watch_gyro(mp_obj_t sample_rate_obj) {
    //int sample_rate = mp_obj_get_int(sample_rate_obj);
    //int delay = 1000/sample_rate;
    while(true){
        sleep(1);
        printf("%f\n", gyroAngle);
    }
    printf("returning\n");
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fll_watch_gyro_obj, fll_watch_gyro);

// micropython stuff ===================================================================================================================
// =====================================================================================================================================

// all functions that can be called from the mp library go here
STATIC const mp_rom_map_elem_t fll_module_globals_table[] = {
{ MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_fllengine) },
{ MP_ROM_QSTR(MP_QSTR_test), MP_ROM_PTR(&fll_test_obj) },
{ MP_ROM_QSTR(MP_QSTR_watchGyro), MP_ROM_PTR(&fll_watch_gyro_obj) },
{ MP_ROM_QSTR(MP_QSTR_startGyro), MP_ROM_PTR(&fll_start_gyro_obj) },
{ MP_ROM_QSTR(MP_QSTR_stopGyro), MP_ROM_PTR(&fll_stop_gyro_obj) },
{ MP_ROM_QSTR(MP_QSTR_toLoc), MP_ROM_PTR(&fll_to_loc_obj) },
{ MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&fll_init_obj) },
};

STATIC MP_DEFINE_CONST_DICT(fll_module_globals, fll_module_globals_table);

const mp_obj_module_t fll_engine_cmodule = {
.base = { &mp_type_module },
.globals = (mp_obj_dict_t *)&fll_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_fllengine, fll_engine_cmodule, 1);
