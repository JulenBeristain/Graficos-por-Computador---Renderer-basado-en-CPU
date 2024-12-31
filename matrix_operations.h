#ifndef MATRIX_OPERATIONS_H
#define MATRIX_OPERATIONS_H

#include "obj.h"

void print_matrix(const char *const str, const real_t *const mptr);
void print_transformation_matrix(const char *const str, const object *const objptr);

void mxp(punto *pptr, const real_t *const m, const punto p);
void mxp3(punto3 *pptr, const real_t *const m, const punto3 p);
void mxm(real_t *const mres, const real_t *const m1, const real_t *const m2);
vector mxv(const real_t *const m, const vector v);

real_t scalar_product(const vector v1, const vector v2);
real_t calculate_vector_module(vector v);
vector normalized(vector v);
vector opposite(vector v);
vector scalar_multiplication(vector v, real_t s);
vector scalar_division(vector v, real_t d);
vector calculate_vector_AtoB(const punto a, const punto b);
vector calculate_vector_AtoB3(const punto3 a, const punto3 b);
vector calculate_vectorial_product(vector v1, vector v2);
vector calculate_normal_vector(const punto3 p1, const punto3 p2, const punto3 p3);
vector calculate_sum_vectors(const vector v1, const vector v2);
vector calculate_subtraction_vectors(const vector v1, const vector v2);

void xyze_to_mobj(real_t *const mptr, const vector x, const vector y, const vector z, const punto3 e);
punto3 mobj_to_e(const real_t *const mptr);
vector mobj_to_x(const real_t *const mptr);
vector mobj_to_y(const real_t *const mptr);
vector mobj_to_z(const real_t *const mptr);

void transpose(real_t *const mtptr, const real_t *const mptr);

#endif