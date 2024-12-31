#ifndef TRASFORMATIONS_H
#define TRASFORMATIONS_H

#include "obj.h"

#define DESPL_TRASL     10
#define ESCAL_AGRANDAR  1.2
#define ESCAL_REDUCIR   (1 / ESCAL_AGRANDAR)
#define PI              3.141593
#define ANG_ROT         (PI / 18)   // 10 grados

typedef void (*TransformationFunc_t)(object*, real_t*) ;

void traslacion_x_local(object *obj_ptr, real_t traslacion);
void traslacion_x_global(object *obj_ptr, real_t traslacion);
void traslacion_x(object *obj_ptr, real_t traslacion, unsigned char is_local);

void traslacion_y_local(object *obj_ptr, real_t traslacion);
void traslacion_y_global(object *obj_ptr, real_t traslacion);
void traslacion_y(object *obj_ptr, real_t traslacion, unsigned char is_local);

void traslacion_z_local(object *obj_ptr, real_t traslacion);
void traslacion_z_global(object *obj_ptr, real_t traslacion);
void traslacion_z(object *obj_ptr, real_t traslacion, unsigned char is_local);


void escalado_local(object *obj_ptr, real_t escala);
void escalado_global(object *obj_ptr, real_t escala);
void escalado(object *obj_ptr, real_t escala, unsigned char is_local);


void rotacion_x_local(object *obj_ptr, real_t angle);
void rotacion_x_global(object *obj_ptr, real_t angle);
void rotacion_x(object *obj_ptr, real_t angle, unsigned char is_local);

void rotacion_y_local(object *obj_ptr, real_t angle);
void rotacion_y_global(object *obj_ptr, real_t angle);
void rotacion_y(object *obj_ptr, real_t angle, unsigned char is_local);

void rotacion_z_local(object *obj_ptr, real_t angle);
void rotacion_z_global(object *obj_ptr, real_t angle);
void rotacion_z(object *obj_ptr, real_t angle, unsigned char is_local);


typedef void (*funcAldaketaObjeto)(object*, int);
void x_aldaketa(object *objptr, int dir);
void y_aldaketa(object *objptr, int dir);
void z_aldaketa(object *objptr, int dir);

void undo(object *objptr);
void undo_list(mlist **mptrptr);

typedef funcAldaketaObjeto funcAldaketaCamaraVuelo;
void x_aldaketa_cam_vuelo(camara *camptr, int dir);
void y_aldaketa_cam_vuelo(camara *camptr, int dir);
void z_aldaketa_cam_vuelo(camara *camptr, int dir);

void lookAt(camara *camptr, punto3 target);

typedef void (*funcAldaketaCamaraAnalisis)(camara*, punto3, int);
void x_aldaketa_cam_analisis(camara *camptr, punto3 center, int dir);
void y_aldaketa_cam_analisis(camara *camptr, punto3 center, int dir);
void z_aldaketa_cam_analisis(camara *camptr, punto3 center, int dir);

typedef void (*funcAldaketaLuzDireccional)(light*, int);
void x_aldaketa_luz_direccional(light *lptr, int dir);
void y_aldaketa_luz_direccional(light *lptr, int dir);
void z_aldaketa_luz_direccional(light *lptr, int dir);

typedef void (*funcAldaketaLuzPosicional)(light*, int);
void x_aldaketa_luz_posicional(light *lptr, int dir);
void y_aldaketa_luz_posicional(light *lptr, int dir);
void z_aldaketa_luz_posicional(light *lptr, int dir);

#endif