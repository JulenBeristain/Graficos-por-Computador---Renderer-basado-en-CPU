#include <stdio.h>
#include <math.h>
#include "matrix_operations.h"
#include <assert.h>


//
// DIBUJAR LA MATRIZ
//
void print_matrix(const char *const str, const real_t *const mptr)
{
    int i;

    printf("%s\n",str);
    for (i = 0;i<4;i++)
        printf(REAL_T_SPECIFIER ", " REAL_T_SPECIFIER ", " REAL_T_SPECIFIER ", " REAL_T_SPECIFIER "\n",
               mptr[i*4], mptr[i*4+1], mptr[i*4+2], mptr[i*4+3]);
}

//
// DIBUJAR LA MATRIZ DE TRANSFORMACIÓN ACTUAL DEL OBJETO DADO
//
void print_transformation_matrix(const char *const str, const object *const objptr)
{
    print_matrix(str, objptr->mptr->m);
}

//
// OPERACIONES ENTRE MATRICES Y: MATRICES, PUNTOS Y VECTORES
//
void mxp(punto *pptr, const real_t *const m, const punto p)
{
    pptr->x = m[0]*p.x + m[1]*p.y + m[2]*p.z  + m[3];
    pptr->y = m[4]*p.x + m[5]*p.y + m[6]*p.z  + m[7];
    pptr->z = m[8]*p.x + m[9]*p.y + m[10]*p.z + m[11];
    pptr->u = p.u;
    pptr->v = p.v;
}

void mxp3(punto3 *pptr, const real_t *const m, const punto3 p)
{
    pptr->x = m[0]*p.x + m[1]*p.y + m[2]*p.z  + m[3];
    pptr->y = m[4]*p.x + m[5]*p.y + m[6]*p.z  + m[7];
    pptr->z = m[8]*p.x + m[9]*p.y + m[10]*p.z + m[11];
}

void mxm(real_t *const mres, const real_t *const m1, const real_t *const m2){
    int i, j, k;
    real_t res;
    for(i = 0; i < 4; ++i){
        for(j = 0; j < 4; ++j){
            res = 0.0;
            for(k = 0; k < 4; ++k){
                res += m1[4*i + k] * m2[4*k + j];
            }
            mres[4*i + j] = res;
        }
    }
}

vector mxv(const real_t *const m, const vector v){
    vector result;
    result.x = m[0]*v.x + m[1]*v.y + m[2]*v.z + m[3];
    result.y = m[4]*v.x + m[5]*v.y + m[6]*v.z + m[7];
    result.z = m[8]*v.x + m[9]*v.y + m[10]*v.z + m[11];
    return result;
}

//
// OPERACIONES ENTRE VECTORES Y PARA CÁLCULO DE VECTORES
//
real_t scalar_product(const vector v1, const vector v2){
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

real_t calculate_vector_module(vector v){
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

vector normalized(vector v) {
    real_t m = calculate_vector_module(v);
    if(m == 0) return (vector) {0, 0, 0};
    return (vector){v.x/m, v.y/m, v.z/m};
}

vector opposite(vector v){
    return (vector){-v.x, -v.y, -v.z};
}

vector scalar_multiplication(vector v, real_t s){
    return (vector){s * v.x, s * v.y, s * v.z};
}

vector scalar_division(vector v, real_t d){
    return (vector){v.x / d, v.y / d, v.z / d};
}

vector calculate_vector_AtoB3(const punto3 a, const punto3 b) {
    return (vector){ b.x - a.x, b.y - a.y, b.z - a.z };
}
vector calculate_vector_AtoB(const punto a, const punto b) {
    return (vector){ b.x - a.x, b.y - a.y, b.z - a.z };
}

vector calculate_vectorial_product(vector v1, vector v2) {
    return (vector) {
        v1.y*v2.z - v1.z*v2.y,
        v1.z*v2.x - v1.x*v2.z,
        v1.x*v2.y - v1.y*v2.x
    };
}

vector calculate_normal_vector(const punto3 p1, const punto3 p2, const punto3 p3) {
    vector v1, v2, n;

    v1 = calculate_vector_AtoB3(p1, p2);
    v2 = calculate_vector_AtoB3(p2, p3);
    n = calculate_vectorial_product(v1, v2);
    return normalized(n);
}

vector calculate_sum_vectors(const vector v1, const vector v2){
    return (vector){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

vector calculate_subtraction_vectors(const vector v1, const vector v2){
    return (vector){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

//
// FUNCIÓN QUE CALCULA LA MATRIZ DE TRANSFORMACIÓN DEL OBJETO A PARTIR DE SUS NUEVOS
// EJES X, Y, Z (en coordenadas del mundo) Y DE SU POSICIÓN
//
void xyze_to_mobj(real_t *const mptr, const vector x, const vector y, const vector z, const punto3 e){
    mptr[0] = x.x; mptr[1] = y.x; mptr[2]  = z.x; mptr[3]  = e.x;
    mptr[4] = x.y; mptr[5] = y.y; mptr[6]  = z.y; mptr[7]  = e.y;
    mptr[8] = x.z; mptr[9] = y.z; mptr[10] = z.z; mptr[11] = e.z;
    mptr[12] = 0;  mptr[13] = 0;  mptr[14] = 0;   mptr[15] = 1;
}

punto3 mobj_to_e(const real_t *const mptr){
    return (punto3){mptr[3], mptr[7], mptr[11]};
}

vector mobj_to_x(const real_t *const mptr){
    return (vector){mptr[0], mptr[4], mptr[8]};
}

vector mobj_to_y(const real_t *const mptr){
    return (vector){mptr[1], mptr[5], mptr[9]};
}

vector mobj_to_z(const real_t *const mptr){
    return (vector){mptr[2], mptr[6], mptr[10]};
}


//
// FUNCIÓN QUE CALCULA LA TRASPUESTA DE UNA MATRIZ
//
void transpose(real_t *const mtptr, const real_t *const mptr){
    unsigned int i, j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            mtptr[4*j+i] = mptr[4*i+j];
        }
    }
}
