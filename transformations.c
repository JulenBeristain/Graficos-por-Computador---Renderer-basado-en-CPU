#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "transformations.h"
#include "matrix_operations.h"
#include "camaras.h"

//
// INICIALIZADORES DE LOS ARRAYS DE TRANSFORMACIÓN DE LOS OBJETOS
//
#define initialize_traslation_x(t) {1, 0, 0, t, \
                                    0, 1, 0, 0, \
                                    0, 0, 1, 0, \
                                    0, 0, 0, 1}

#define initialize_traslation_y(t) {1, 0, 0, 0, \
                                    0, 1, 0, t, \
                                    0, 0, 1, 0, \
                                    0, 0, 0, 1}

#define initialize_traslation_z(t) {1, 0, 0, 0, \
                                    0, 1, 0, 0, \
                                    0, 0, 1, t, \
                                    0, 0, 0, 1}

#define initialize_traslation(x, y, z) {1, 0, 0, x, \
                                        0, 1, 0, y, \
                                        0, 0, 1, z, \
                                        0, 0, 0, 1}


#define initialize_scale(s) {s, 0, 0, 0, \
                             0, s, 0, 0, \
                             0, 0, s, 0, \
                             0, 0, 0, 1}


//c: coseno del angulo de rotación
//s: seno
#define initialize_rotation_x(c, s) {1, 0, 0,  0, \
                                     0, c, -s, 0, \
                                     0, s,  c, 0, \
                                     0, 0,  0, 1}

#define initialize_rotation_y(c, s) {c, 0, s, 0, \
                                     0, 1, 0, 0, \
                                    -s, 0, c, 0, \
                                     0, 0, 0, 1}

#define initialize_rotation_z(c, s) {c, -s, 0, 0, \
                                     s,  c, 0, 0, \
                                     0,  0, 1, 0, \
                                     0,  0, 0, 1}

//
//FUNCIONES QUE INTRODUCEN TRANSFORMACIONES A MLIST
// EZK: transformación en sistema de referencia global
// ESK:                                         local
//
void aldaketa_sartu_ezk(mlist **mptrptr, real_t mtransf[16]){
    mlist *newmlist = (mlist*) malloc (sizeof(mlist));
    mxm(newmlist->m, mtransf, (*mptrptr)->m);
    newmlist->hptr = *mptrptr;
    *mptrptr = newmlist;
}

void aldaketa_sartu_esk(mlist **mptrptr, real_t mtransf[16]){
    mlist *newmlist = (mlist*) malloc (sizeof(mlist));
    mxm(newmlist->m, (*mptrptr)->m, mtransf);
    newmlist->hptr = *mptrptr;
    *mptrptr = newmlist;
}

//
// FUNCIONES QUE INTRODUCEN TRANSFORMACIONES A LOS OBJETOS
// EZK: transformación en sistema de referencia global
// ESK:                                         local
// (se podrían poner en función de aldaketa_sartu_ezk/esk)
void objektuari_aldaketa_sartu_ezk(object *obj_ptr, real_t mtransf[16]){
    mlist *newmlist = (mlist*) malloc (sizeof(mlist));
    mxm(newmlist->m, mtransf, obj_ptr->mptr->m);
    newmlist->hptr = obj_ptr->mptr;
    obj_ptr->mptr = newmlist;
}

void objektuari_aldaketa_sartu_esk(object *obj_ptr, real_t mtransf[16]){
    mlist *newmlist = (mlist*) malloc (sizeof(mlist));
    mxm(newmlist->m, obj_ptr->mptr->m, mtransf);
    newmlist->hptr = obj_ptr->mptr;
    obj_ptr->mptr = newmlist;
}


//
// FUNCIONES DE TRASLACION
//
void traslacion_x_aux(object *obj_ptr, real_t t, TransformationFunc_t transf_func){
    real_t mtraslacion[] = initialize_traslation_x(t);
    transf_func(obj_ptr, mtraslacion);
}
void traslacion_x_local(object *obj_ptr, real_t traslacion) {
    traslacion_x_aux(obj_ptr, traslacion, objektuari_aldaketa_sartu_esk);
}
void traslacion_x_global(object *obj_ptr, real_t traslacion) {
    traslacion_x_aux(obj_ptr, traslacion, objektuari_aldaketa_sartu_ezk);
}
void traslacion_x(object *obj_ptr, real_t traslacion, unsigned char is_local) {
    if (is_local) traslacion_x_local(obj_ptr, traslacion);
    else          traslacion_x_global(obj_ptr, traslacion);
}


void traslacion_y_aux(object *obj_ptr, real_t t, TransformationFunc_t transf_func){
    real_t mtraslacion[] = initialize_traslation_y(t);
    transf_func(obj_ptr, mtraslacion);
}
void traslacion_y_local(object *obj_ptr, real_t traslacion) {
    traslacion_y_aux(obj_ptr, traslacion, objektuari_aldaketa_sartu_esk);
}
void traslacion_y_global(object *obj_ptr, real_t traslacion) {
    traslacion_y_aux(obj_ptr, traslacion, objektuari_aldaketa_sartu_ezk);
}
void traslacion_y(object *obj_ptr, real_t traslacion, unsigned char is_local){
    if (is_local) traslacion_y_local(obj_ptr, traslacion);
    else          traslacion_y_global(obj_ptr, traslacion);
}


void traslacion_z_aux(object *obj_ptr, real_t t, TransformationFunc_t transf_func){
    real_t mtraslacion[] = initialize_traslation_z(t);
    transf_func(obj_ptr, mtraslacion);
}
void traslacion_z_local(object *obj_ptr, real_t traslacion) {
    traslacion_z_aux(obj_ptr, traslacion, objektuari_aldaketa_sartu_esk);
}
void traslacion_z_global(object *obj_ptr, real_t traslacion) {
    traslacion_z_aux(obj_ptr, traslacion, objektuari_aldaketa_sartu_ezk);
}
void traslacion_z(object *obj_ptr, real_t traslacion, unsigned char is_local){
    if (is_local) traslacion_z_local(obj_ptr, traslacion);
    else          traslacion_z_global(obj_ptr, traslacion);
}


//
// FUNCIONES DE ESCALADO
//
void escalado_aux(object *obj_ptr, real_t s, TransformationFunc_t transf_func){
    real_t mscale[] =  initialize_scale(s);
    transf_func(obj_ptr, mscale);
}
void escalado_local(object *obj_ptr, real_t escala) {
    escalado_aux(obj_ptr, escala, objektuari_aldaketa_sartu_esk);
}
void escalado_global(object *obj_ptr, real_t escala) {
    escalado_aux(obj_ptr, escala, objektuari_aldaketa_sartu_ezk);
}
void escalado(object *obj_ptr, real_t escala, unsigned char is_local){
    if (is_local) escalado_local(obj_ptr, escala);
    else          escalado_global(obj_ptr, escala);
}


//
// FUNCIONES DE ROTACION
//
void rotacion_x_aux(object *obj_ptr, real_t angle, TransformationFunc_t transf_func){
    real_t mrotation[] = initialize_rotation_x(cos(angle), sin(angle));
    transf_func(obj_ptr, mrotation);
}
void rotacion_x_local(object *obj_ptr, real_t angle){
    rotacion_x_aux(obj_ptr, angle, objektuari_aldaketa_sartu_esk);
}
void rotacion_x_global(object *obj_ptr, real_t angle){
    rotacion_x_aux(obj_ptr, angle, objektuari_aldaketa_sartu_ezk);
}
void rotacion_x(object *obj_ptr, real_t angle, unsigned char is_local){
    if (is_local) rotacion_x_local(obj_ptr, angle);
    else          rotacion_x_global(obj_ptr, angle);
}


void rotacion_y_aux(object *obj_ptr, real_t angle, TransformationFunc_t transf_func){
    real_t mrotation[] = initialize_rotation_y(cos(angle), sin(angle));
    transf_func(obj_ptr, mrotation);
}
void rotacion_y_local(object *obj_ptr, real_t angle){
    rotacion_y_aux(obj_ptr, angle, objektuari_aldaketa_sartu_esk);
}
void rotacion_y_global(object *obj_ptr, real_t angle){
    rotacion_y_aux(obj_ptr, angle, objektuari_aldaketa_sartu_ezk);
}
void rotacion_y(object *obj_ptr, real_t angle, unsigned char is_local){
    if (is_local) rotacion_y_local(obj_ptr, angle);
    else          rotacion_y_global(obj_ptr, angle);
}


void rotacion_z_aux(object *obj_ptr, real_t angle, TransformationFunc_t transf_func){
    real_t mrotation[] = initialize_rotation_z(cos(angle), sin(angle));
    transf_func(obj_ptr, mrotation);
}
void rotacion_z_local(object *obj_ptr, real_t angle){
    rotacion_z_aux(obj_ptr, angle, objektuari_aldaketa_sartu_esk);
}
void rotacion_z_global(object *obj_ptr, real_t angle){
    rotacion_z_aux(obj_ptr, angle, objektuari_aldaketa_sartu_ezk);
}
void rotacion_z(object *obj_ptr, real_t angle, unsigned char is_local){
    if (is_local) rotacion_z_local(obj_ptr, angle);
    else          rotacion_z_global(obj_ptr, angle);
}


//
// FUNCIONES PRINCIPALES USADAS POR TEKLATUA
//

//
// VARIABLES GLOBALES QUE DEFINEN LA TRANSFORMACIÓN A APLICAR
//
extern char aldaketa;
extern unsigned char ald_lokala;

// OBJETOS
void x_aldaketa(object *objptr, int dir) {
    real_t traslacion, escala, angle;
    switch(aldaketa){
        case 't':
            traslacion = dir ? DESPL_TRASL : -DESPL_TRASL;
            traslacion_x(objptr, traslacion, ald_lokala);
            break;
        case 'r':
            angle = dir ? ANG_ROT : -ANG_ROT;
            rotacion_x(objptr, angle, ald_lokala);
            break;
        case 's':
            escala = dir ? ESCAL_AGRANDAR : ESCAL_REDUCIR;
            escalado(objptr, escala, ald_lokala);
            break;
        default:
            printf("[x_aldaketa] Valor indefinido de aldaketa: %c", aldaketa);
    }
}

void y_aldaketa(object *objptr, int dir) {
    real_t traslacion, escala, angle;
    switch(aldaketa){
        case 't':
            traslacion = dir ? DESPL_TRASL : -DESPL_TRASL;
            traslacion_y(objptr, traslacion, ald_lokala);
            break;
        case 'r':
            angle = dir ? ANG_ROT : -ANG_ROT;
            rotacion_y(objptr, angle, ald_lokala);
            break;
        case 's':
            escala = dir ? ESCAL_AGRANDAR : ESCAL_REDUCIR;
            escalado(objptr, escala, ald_lokala);
            break;
        default:
            printf("[x_aldaketa] Valor indefinido de aldaketa: %c", aldaketa);
    }
}

void z_aldaketa(object *objptr, int dir) {
    real_t traslacion, escala, angle;
    switch(aldaketa){
        case 't':
            traslacion = dir ? DESPL_TRASL : -DESPL_TRASL;
            traslacion_z(objptr, traslacion, ald_lokala);
            break;
        case 'r':
            angle = dir ? ANG_ROT : -ANG_ROT;
            rotacion_z(objptr, angle, ald_lokala);
            break;
        case 's':
            escala = dir ? ESCAL_AGRANDAR : ESCAL_REDUCIR;
            escalado(objptr, escala, ald_lokala);
            break;
        default:
            printf("[x_aldaketa] Valor indefinido de aldaketa: %c", aldaketa);
    }
}

// TANTO PARA OBJETOS COMO PARA CÁMARAS
// (se puede implementar en función de undo_list)
void undo(object *objptr)
{
    mlist *actual = objptr->mptr;
    //Si la primera matriz de transformación del objeto seleccionado fuera la identidad inicial, no hacer nada
    if(actual->hptr != 0){
        objptr->mptr = actual->hptr;
        free(actual);
    }
}

// PARA CUALQUIER TIPO DE STRUCT QUE CONTENGA UN MLIST
void undo_list(mlist **mptrptr){
    mlist *actual = *mptrptr;
    //Si la primera matriz de transformación de la lista fuera la identidad inicial, no hacer nada
    if(actual->hptr != 0){
        *mptrptr = actual->hptr;
        free(actual);
    }
}

// CAMARAS
// MODO VUELO:      TODAS LAS TRANSFORMACIONES A LAS CAMARAS EN SU SISTEMA DE REFERENCIA LOCAL
// MODO ANALISIS:   EN EL SISTEMA DE REFERENCIA GLOBAL
// LAS CAMARAS NO SE ESCALAN

//
// TRANSFORMACIONES EN MODO VUELO
//
void x_aldaketa_cam_vuelo(camara *camptr, int dir){
    real_t angle;

    if(aldaketa == 't' || aldaketa == 'r'){
        angle = dir ? ANG_ROT : -ANG_ROT;
        rotacion_y_local(camptr, angle);  //yaw, mirar a izq/der
    }
}

void y_aldaketa_cam_vuelo(camara *camptr, int dir){
    real_t angle;

    if(aldaketa == 't' || aldaketa == 'r'){
        angle = dir ? ANG_ROT : -ANG_ROT;
        rotacion_x_local(camptr, angle);  //pitch, mirar a arriba/abajo
    }
}

void z_aldaketa_cam_vuelo(camara *camptr, int dir){
    real_t traslacion, angle;

    if(aldaketa == 't'){
        traslacion = dir ? DESPL_TRASL : -DESPL_TRASL;
        traslacion_z_local(camptr, traslacion); // avanzar/retroceder
    } else if (aldaketa == 'r'){
        angle = dir ? ANG_ROT : -ANG_ROT;
        rotacion_z_local(camptr, angle);  //roll, rotar en eje z
    }
}

//
// TRANSFORMACIONES EN MODO ANALISIS
//
void lookAt(camara *camptr, punto3 target){
    punto3 cam_pos;
    const real_t *const mptr = camptr->mptr->m;
    vector forward, up, right;
    mlist *newmlist;

    // Calculo de la posicion de la camara en coordenadas del mundo
    // y de los nuevos ejes x,y,z para que la camara apunte al target
    cam_pos = mobj_to_e(mptr);
    forward = normalized(calculate_vector_AtoB3(target, cam_pos));
    up = (vector){0, 1, 0};
    right = normalized(calculate_vectorial_product(up, forward));
    up = calculate_vectorial_product(forward, right); // ya normalizado

    // Calculo de la nueva matriz de transformacion para la camara
    // y añadirlo al principio de la lista de transformaciones
    newmlist = (mlist*) malloc(sizeof(mlist));
    xyze_to_mobj(newmlist->m, right, up, forward, cam_pos);
    newmlist->hptr = camptr->mptr;
    camptr->mptr = newmlist;
}

//
// FUNCIÓN AUXILIAR PARA CALCULAR LA MATRIZ DE TRANSFORMACIÓN EN EL MODO ANALISIS
//
void calculate_analisis_transformation(
    real_t *const mresult, const real_t *const mrotation, real_t *const mtraslation)
{
    real_t maux[16];
    mxm(maux, mrotation, mtraslation);
    mtraslation[3] *= -1; mtraslation[7] *= -1; mtraslation[11] *= -1;
    mxm(mresult, mtraslation, maux);
}
//
// FUNCIÓN AUXILIAR PARA CALCULAR LA MATRIZ DE ROTACIÓN ALREDEDOR DE CUALQUIER EJE
// Precondición : axis es unitario
//
void rotacion_global(real_t *const mptr, const vector axis, const real_t angle){
    real_t c, s; //cos y sin
    real_t sqrt_1_z_2;
    real_t mtemp[16];

    //caso especial: axis.x = axis.y = 0 --> sqrt(1 - axis.z^2) = 0 --> axis.z = 1 --> rotación eje z
    if((sqrt_1_z_2 = sqrt(1 - axis.z*axis.z)) == 0){
        real_t mrotation[] = initialize_rotation_z(cos(angle), sin(angle));
        memcpy(mptr, mrotation, sizeof(mrotation));
        return;
    }

    //primero: rotamos alrededor del eje z --> llevamos el punto al plano YZ
    c = axis.y / sqrt_1_z_2;
    s = axis.x / sqrt_1_z_2;
    real_t mrotationz[] = initialize_rotation_z(c, s);

    //segundo: rotamos alrededor del eje x --> llevamos el punto al plano XZ
    c = axis.z;
    s = sqrt_1_z_2;
    real_t mrotationx[] = initialize_rotation_x(c, s);

    //tercero: rotamos el angulo deseado alrededor del eje z
    real_t mrotation[] = initialize_rotation_z(cos(angle), sin(angle));

    //cuarto: deshacemos la segunda rotación (alrededor del eje x)
    real_t minvrotationx[16];
    transpose(minvrotationx, mrotationx);

    //quinto: deshacemos la primera rotación (alrededor del eje z)
    real_t minvrotationz[16];
    transpose(minvrotationz, mrotationz);

    //Por último: empleamos una matriz temporal para calcular el producto entre las cinco matrices de rotación
    mxm(mtemp, mrotationx, mrotationz);
    mxm(mptr, mrotation, mtemp);
    mxm(mtemp, minvrotationx, mptr);
    mxm(mptr, minvrotationz, mtemp);
}
//
// VARIANTE UTILIZANDO LA FORMULA DE RODRIGUES
//
void calculateRotationMatrix(real_t *const rotationMatrix, real_t axisX, real_t axisY, real_t axisZ, real_t angle) {

    // Calculate trigonometric values
    real_t cosTheta = cos(angle);
    real_t sinTheta = sin(angle);
    real_t oneMinusCosTheta = 1.0f - cosTheta;

    // Fill in the rotation matrix
    rotationMatrix[0] = cosTheta + axisX * axisX * oneMinusCosTheta;
    rotationMatrix[1] = axisX * axisY * oneMinusCosTheta - axisZ * sinTheta;
    rotationMatrix[2] = axisX * axisZ * oneMinusCosTheta + axisY * sinTheta;
    rotationMatrix[3] = 0.0f;

    rotationMatrix[4] = axisY * axisX * oneMinusCosTheta + axisZ * sinTheta;
    rotationMatrix[5] = cosTheta + axisY * axisY * oneMinusCosTheta;
    rotationMatrix[6] = axisY * axisZ * oneMinusCosTheta - axisX * sinTheta;
    rotationMatrix[7] = 0.0f;

    rotationMatrix[8] = axisZ * axisX * oneMinusCosTheta - axisY * sinTheta;
    rotationMatrix[9] = axisZ * axisY * oneMinusCosTheta + axisX * sinTheta;
    rotationMatrix[10] = cosTheta + axisZ * axisZ * oneMinusCosTheta;
    rotationMatrix[11] = 0.0f;

    rotationMatrix[12] = 0.0f;
    rotationMatrix[13] = 0.0f;
    rotationMatrix[14] = 0.0f;
    rotationMatrix[15] = 1.0f;
}


//
// FUNCIONES PRINCIPALES LLAMADAS EN TEKLATUA PARA TRANSFORMACIONES DE CAMARA EN MODO ANALISIS
//
void x_aldaketa_cam_analisis(camara *camptr, punto3 center, int dir){
    vector axis;
    real_t angle;
    real_t mrotation[16];
    
    if(aldaketa != 't' && aldaketa != 'r') return;

    real_t mtraslation[16] = initialize_traslation(-center.x, -center.y, -center.z);
    
    //rotación y de la cámara
    angle = dir ? ANG_ROT : -ANG_ROT;
    axis = normalized(mobj_to_y(camptr->mptr->m));
    //rotacion_global(mrotation, axis, angle);
    calculateRotationMatrix(mrotation, axis.x, axis. y, axis.z, angle);
    
    real_t mcomplete[16];
    calculate_analisis_transformation(mcomplete, mrotation, mtraslation);
    objektuari_aldaketa_sartu_ezk(camptr, mcomplete);
}

void y_aldaketa_cam_analisis(camara *camptr, punto3 center, int dir){
    vector axis;
    real_t angle;
    real_t mrotation[16];
    
    if(aldaketa != 't' && aldaketa != 'r') return;

    real_t mtraslation[16] = initialize_traslation(-center.x, -center.y, -center.z);
    
    //rotación eje x cámara
    angle = dir ? ANG_ROT : -ANG_ROT;
    axis = normalized(mobj_to_x(camptr->mptr->m));
    //rotacion_global(mrotation, axis, angle);
    calculateRotationMatrix(mrotation, axis.x, axis.y, axis.z, angle);

    real_t mcomplete[16];
    calculate_analisis_transformation(mcomplete, mrotation, mtraslation);
    objektuari_aldaketa_sartu_ezk(camptr, mcomplete);
}

void z_aldaketa_cam_analisis(camara *camptr, punto3 center, int dir){
    real_t traslacion, angle;
    const real_t *const mptr = camptr->mptr->m;
    punto3 cam_pos;

    if(aldaketa == 't'){
        traslacion = dir ? DESPL_TRASL : -DESPL_TRASL;
        //en caso de acercarnos al centro del objeto, no podemos sobrepasarlo
        if(traslacion < 0){
            cam_pos = mobj_to_e(mptr);
            if(calculate_vector_module(calculate_vector_AtoB3(center, cam_pos)) <= -traslacion){
                return;
            }
        }
        traslacion_z_local(camptr, traslacion); // avanzar/retroceder
    } else if (aldaketa == 'r'){
        angle = dir ? ANG_ROT : -ANG_ROT;
        rotacion_z_local(camptr, angle);  //roll, rotar en eje z
    }
}


//
// FUNCIONES PRINCIPALES LLAMADAS EN TEKLATUA PARA TRANSFORMACIONES DE LUCES DIRECCIONALES Y POSICIONALES
// 

// Direccionales: sol
void x_aldaketa_luz_direccional(light *lptr, int dir){
    real_t angle = dir ? ANG_ROT : -ANG_ROT;
    real_t mrotation[] = initialize_rotation_x(cos(angle), sin(angle));
    
    //Da igual meterlo por la derecha que por la izquierda, ya que es una rotación a un vector
    aldaketa_sartu_ezk(&(lptr->mptr), mrotation);
}
void y_aldaketa_luz_direccional(light *lptr, int dir){
    real_t angle = dir ? ANG_ROT : -ANG_ROT;
    real_t mrotation[] = initialize_rotation_y(cos(angle), sin(angle));
    
    //Da igual meterlo por la derecha que por la izquierda, ya que es una rotación a un vector
    aldaketa_sartu_ezk(&(lptr->mptr), mrotation);
}
void z_aldaketa_luz_direccional(light *lptr, int dir){
    real_t angle = dir ? ANG_ROT : -ANG_ROT;
    real_t mrotation[] = initialize_rotation_z(cos(angle), sin(angle));
    
    //Da igual meterlo por la derecha que por la izquierda, ya que es una rotación a un vector
    aldaketa_sartu_ezk(&(lptr->mptr), mrotation);
}

//Posicionales: bombilla
void x_aldaketa_luz_posicional(light *lptr, int dir){
    real_t t = dir ? DESPL_TRASL : -DESPL_TRASL;
    real_t mtraslation[] = initialize_traslation_x(t);
    
    //Da igual meterlo por la derecha que por la izquierda, ya que es una traslación a un punto (que no rota)
    aldaketa_sartu_ezk(&(lptr->mptr), mtraslation);
}
void y_aldaketa_luz_posicional(light *lptr, int dir){
    real_t t = dir ? DESPL_TRASL : -DESPL_TRASL;
    real_t mtraslation[] = initialize_traslation_y(t);
    
    //Da igual meterlo por la derecha que por la izquierda, ya que es una traslación a un punto (que no rota)
    aldaketa_sartu_ezk(&(lptr->mptr), mtraslation);
}
void z_aldaketa_luz_posicional(light *lptr, int dir){
    real_t t = dir ? DESPL_TRASL : -DESPL_TRASL;
    real_t mtraslation[] = initialize_traslation_z(t);
    
    //Da igual meterlo por la derecha que por la izquierda, ya que es una traslación a un punto (que no rota)
    aldaketa_sartu_ezk(&(lptr->mptr), mtraslation);
}