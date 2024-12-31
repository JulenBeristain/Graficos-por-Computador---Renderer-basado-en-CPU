#ifndef LOAD_OBJ_H
#define LOAD_OBJ_H

#include "obj.h"

void load_from_file(
    const char *const file_name, object **const firstptrptr, object **const selptrptr,
    const color3 ka, const color3 kd, const color3 ks, const real_t ns);

#endif // LOAD_OBJ_H