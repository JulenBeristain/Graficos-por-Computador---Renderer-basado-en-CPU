#ifndef DIBUJAR_TRIANGULO_H
#define DIBUJAR_TRIANGULO_H

#include "obj.h"

//
// FUNCIONES QUE DIBUJAN LOS TRIÁNGULOS
//
void dibujar_perimetro_triangulo(const punto3 p1, const punto3 p2, const punto3 p3, const color3 rgb);
void dibujar_triangulo_color(punto3 p1, punto3 p2, punto3 p3, const color3 rgb);
void dibujar_triangulo_iluminado(punto3 p1, punto3 p2, punto3 p3, color3 I1, color3 I2, color3 I3);

#endif