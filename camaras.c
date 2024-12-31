//#include <stdarg.h>
#include "camaras.h"
#include "matrix_operations.h"

/*
// Matriz de proyección en perspectiva
const double perspective_proyection_m[16] = {
    2*NEAR_ / (RIGHT - LEFT), 0                      , (RIGHT + LEFT) / (RIGHT - LEFT), 0,
    0                      , 2*NEAR_ / (TOP - BOTTOM), (TOP + BOTTOM) / (TOP - BOTTOM), 0,
    0                      , 0                      , -(FAR_ + NEAR_) / (FAR_ - NEAR_)   , -2*FAR_*NEAR_ / (FAR_ - NEAR_),
    0                      , 0                      , -1                             , 0
};

//Matriz de proyección ortográfica
const double ortographic_proyection_m[16] = {
    2.0f / (ORIGHT - OLEFT), 0, 0, -(ORIGHT + OLEFT) / (ORIGHT - OLEFT),
    0, 2.0f / (OTOP - OBOTTOM), 0, -(OTOP + OBOTTOM) / (OTOP - OBOTTOM),
    0, 0, -2.0f / (OFAR - ONEAR),  -(OFAR + ONEAR) / (OFAR - ONEAR),
    0, 0, 0, 1
};

double mcsr[16];
*/

//
// CÁLCULO DE LA MATRIZ DE CAMBIO DE SISTEMA DE REFERENCIA
//
void mobj_to_mcsr(real_t *const mcsr, const real_t *const mobj){
    vector xobj = (vector){mobj[0], mobj[4], mobj[8]};
    vector yobj = (vector){mobj[1], mobj[5], mobj[9]};
    vector zobj = (vector){mobj[2], mobj[6], mobj[10]};
    vector eobj = (vector){mobj[3], mobj[7], mobj[11]};

    mcsr[0] = xobj.x; mcsr[1] = xobj.y; mcsr[2]  = xobj.z; mcsr[3]  = - scalar_product(eobj, xobj);
    mcsr[4] = yobj.x; mcsr[5] = yobj.y; mcsr[6]  = yobj.z; mcsr[7]  = - scalar_product(eobj, yobj);
    mcsr[8] = zobj.x; mcsr[9] = zobj.y; mcsr[10] = zobj.z; mcsr[11] = - scalar_product(eobj, zobj);
    mcsr[12] = 0; mcsr[13] = 0; mcsr[14] = 0; mcsr[15] = 1;
}

//
// FUNCIONES PARA PASAR DE NUESTRAS COORDENADAS (entre -500 y 500) A NDC
//
/*
extern unsigned char proyection;

//num: indica el número de puntos que hay que convertir en NDC
void calculateNDCPoints(int num, ...){
    va_list args;
    va_start(args, num);
    const real_t *const mproy_ptr = proyection == PARALELO ? ortographic_proyection_m : perspective_proyection_m;
    real_t w;
    
    punto3 p, *ndc_p_ptr;
    int i;
    for(i = 0; i < num; ++i){
        ndc_p_ptr = va_arg(args, punto3*);
        p = va_arg(args, punto3);
        mxp(ndc_p_ptr, mproy_ptr, p);
        
        // * Cuidado con valores cercanos a 0
        // Sabemos que los dos primeros valores en la última fila de las dos matrices de proyección son 0
        //w = mproy_ptr[12]*p.x + mproy_ptr[13]*p.y + mproy_ptr[14]*p.z + mproy_ptr[15];
        w = mproy_ptr[14]*p.z + mproy_ptr[15];
        ndc_p_ptr->x /= w;
        ndc_p_ptr->y /= w;
        ndc_p_ptr->z /= w;
    }

    va_end(args);
}

//
// FUNCIONES INTERFAZ DONDE EL USUARIO NO SE PREOCUPA DE TENER QUE ESPECIFICAR BIEN NUM
//
void calculateNDCp1(punto3 *ndcptr, punto3 p){
    calculateNDCPoints(1, ndcptr, p);
}

void calculateNDCp2(punto3 *ndcptr1, punto3 p1, punto3 *ndcptr2, punto3 p2){
    calculateNDCPoints(2, ndcptr1, p1, ndcptr2, p2);
}

void calculateNDCp3(punto3 *ndcptr1, punto3 p1, punto3 *ndcptr2, punto3 p2, punto3 *ndcptr3, punto3 p3){
    calculateNDCPoints(3, ndcptr1, p1, ndcptr2, p2, ndcptr3, p3);
}
*/