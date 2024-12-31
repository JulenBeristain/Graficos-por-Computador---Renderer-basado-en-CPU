#ifndef CAMARAS_H
#define CAMARAS_H

#include "obj.h"

// Matriz de proyección en perspectiva
#define NEAR_     1.0f               
#define FAR_      2000.0f
#define LEFT     (-RIGHT)
#define RIGHT    (NEAR_ * 0.4142136f)    //(tan(PI / 8.0)), 45º / 2
#define BOTTOM   (-TOP)
#define TOP      (NEAR_ * 0.4142136f)    //(tan(PI / 8.0)), 45º / 2

//Matriz de proyección ortográfica
#define ONEAR       0.0f
#define OFAR        2000.0f
#define OLEFT       (-ORIGHT)
#define ORIGHT      500.0f
#define OBOTTOM     (-OTOP)
#define OTOP        500.0f

//Valores para definir opciones de cámara
enum { CAMARA = 0, OBJETO = 1 };
enum { PARALELO = 0, PERSPECTIVA = 1 };
enum { ANALISIS = 0, VUELO = 1 };

/*
extern const double perspective_proyection_m[16];
extern const double ortographic_proyection_m[16];
extern double mcsr[16];
*/

void mobj_to_mcsr(real_t *const mcsr, const real_t *const mobj);

/*
void calculateNDCp1(punto3 *ndcptr, punto3 p);
void calculateNDCp2(punto3 *ndcptr1, punto3 p1, punto3 *ndcptr2, punto3 p2);
void calculateNDCp3(punto3 *ndcptr1, punto3 p1, punto3 *ndcptr2, punto3 p2, punto3 *ndcptr3, punto3 p3);
*/

#endif