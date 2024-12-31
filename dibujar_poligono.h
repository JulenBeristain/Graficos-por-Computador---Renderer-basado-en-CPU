#ifndef DIBUJAR_POLIGONO_H
#define DIBUJAR_POLIGONO_H

#include "obj.h"

typedef void (*funcMostrarNormalCara)(const punto3, const punto3, const color3);
void mostrar_normal_cara(const punto3 p1, const punto3 p2, const color3 rgb);
void no_mostrar_normal_cara(const punto3 p1, const punto3 p2, const color3 rgb);

typedef void (*funcMostrarNormalVertice)(const punto3, const punto3, const color3);
void mostrar_normal_vertice(const punto3 p1, const punto3 p2, const color3 rgb);
void no_mostrar_normal_vertice(const punto3 p1, const punto3 p2, const color3 rgb);

extern funcMostrarNormalCara f_mostrar_normal_cara;
extern funcMostrarNormalVertice f_mostrar_normal_vertice;


typedef bool (*funcTestBackwards)(const punto3 center, const vector normal);
bool is_looking_backwards_paralelo(const punto3 center, const vector normal);
bool is_looking_backwards_perspectiva(const punto3 center, const vector normal);
extern funcTestBackwards f_is_looking_backwards;


// Typedef de las funciones de dibujado de poligonos
typedef void (*funcDibujarPoligono)(
    const object *const, const face *const, 
    const real_t *const, const real_t *const);

extern const funcDibujarPoligono tabla_dibujar_poligono[];
unsigned int indice_dibujar_poligono();

#endif // DIBUJAR_POLIGONO_H