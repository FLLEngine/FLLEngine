#include "functions.h"

#include "py/runtime.h"
#include "py/objstr.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

mp_obj_t fll_sleep(mp_obj_t time_obj) {
    float time = mp_obj_get_float(time_obj);
    usleep(time*1000000);
    return mp_const_true;
}
