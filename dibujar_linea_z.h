#ifndef DIBUJAR_LINEA_Z_H
#define DIBUJAR_LINEA_Z_H

#include "obj.h"

//
// DIBUJAR LÍNEAS HORIZONTALES
//

void  dibujar_linea_z_iluminado(
    real_t linea, real_t c1x, real_t c1z, real_t c2x, real_t c2z,
    const color3 I1, const color3 I2);

void dibujar_linea_z_color(real_t linea, real_t c1x, real_t c1z, real_t c2x, real_t c2z, const color3 rgbptr);

#endif