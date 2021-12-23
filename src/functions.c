#include "functions.h"

#include "py/runtime.h"
#include "py/objstr.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

mp_obj_t fll_sleep(mp_obj_t time_obj) {
    int time = mp_obj_get_int(time_obj);
    sleep(time);
    return mp_const_true;
}
