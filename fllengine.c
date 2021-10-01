#include "py/runtime.h"

#include <stdio.h>
#include <string.h>

const char init_line[] = "Go Mindbenders!";

STATIC mp_obj_t fll_test() {
    printf("%s\n", init_line);
    return mp_const_true;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(fll_test_obj, fll_test);

STATIC const mp_rom_map_elem_t fll_module_globals_table[] = {
{ MP_ROM_QSTR(MP_QSTR__name__), MP_ROM_QSTR(MP_QSTR_fllengine) },
{ MP_ROM_QSTR(MP_QSTR_fll_test), MP_ROM_PTR(&fll_test_obj) },
};

STATIC MP_DEFINE_CONST_DICT(fll_module_globals, fll_module_globals_table);

const mp_obj_module_t fll_engine_cmodule = {
.base = { &mp_type_module },
.globals = (mp_obj_dict_t *)&fll_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_fllengine, fll_engine_cmodule, 1);
