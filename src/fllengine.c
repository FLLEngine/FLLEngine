#include "py/runtime.h"
#include "py/objstr.h"

#include "fllengine.h"
#include "find.h"
#include "motion.h"
#include "functions.h"


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



STATIC mp_obj_t fll_init(mp_obj_t motor_1_obj, mp_obj_t wheel_dia_obj, mp_obj_t invert_obj) {
    int motor1Port = mp_obj_get_int(motor_1_obj);
    float wheelDiameter = mp_obj_get_float(wheel_dia_obj);
    printf("%d", mp_obj_is_true(invert_obj));
    motion_init(motor1Port, wheelDiameter, mp_obj_is_true(invert_obj));
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(fll_init_obj, fll_init);



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



STATIC mp_obj_t fll_drive_straight(mp_obj_t distance_obj, mp_obj_t angle_obj) {
    int distance = mp_obj_get_int(distance_obj);
    float angle = mp_obj_get_float(angle_obj);
    driveStraight(distance, angle);
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(fll_drive_straight_obj, fll_drive_straight);



STATIC mp_obj_t fll_turn_angle(mp_obj_t angle_obj) {
    float angle = mp_obj_get_float(angle_obj);
    turnAngle(angle);
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fll_turn_angle_obj, fll_turn_angle);



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

STATIC MP_DEFINE_CONST_FUN_OBJ_1(fll_sleep_obj, fll_sleep);
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
{ MP_ROM_QSTR(MP_QSTR_driveStraight), MP_ROM_PTR(&fll_drive_straight_obj) },
{ MP_ROM_QSTR(MP_QSTR_sleep), MP_ROM_PTR(&fll_sleep_obj) },
{ MP_ROM_QSTR(MP_QSTR_turnAngle), MP_ROM_PTR(&fll_turn_angle_obj) },
};

STATIC MP_DEFINE_CONST_DICT(fll_module_globals, fll_module_globals_table);

const mp_obj_module_t fll_engine_cmodule = {
.base = { &mp_type_module },
.globals = (mp_obj_dict_t *)&fll_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_fllengine, fll_engine_cmodule, 1);
