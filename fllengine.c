#include "py/runtime.h"

#include <stdio.h>

STATIC mp_obj_t fll_test(mp_obj_t text_obj){

    char txt[] = mp_obj_get_str(text_obj);

    printf("%s\n", txt);
};

STATIC MP_DEFINE_CONST_FUN_OBJ_2(fll_test_obj, fll_test);

STATIC const mp_rom_mapp_elem_t fll_module_globals_table[] = {
{ MP_ROM_QSTR(MP_QSTR__name__), MP_ROM_QSTR(MP_QSTR_fllengine) },
{ MP_ROM_QSTR(MP_QSTR_fll_test), MP_ROM_PTR(&fll_test_obj) },
};

STATIC MP_DEFINE_CONST_DICT(fll_module_globals, example_module_globals_table);

const mp_obj_module_t fll_engine_cmodule = {
.base = { $mp_type_module },
.globals = (mp_obj_dict_t *)&fll_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_fllengine, fll_engine_cmodule, 1);
