#include <GL/glut.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "dibujar_triangulo.h"
#include "matrix_operations.h"
#include "dibujar_linea_z.h"
//#include "camaras.h"

#define YCHANGE 0.1

//
// FUNCIONES AUXILIARES PARA DELIMITAR LOS PUNTOS DE LOS TRIÁNGULOS A DIBUJAR
//

// (*)
// Se cumple la precondición de que (psupptr->y - pinfptr->y) != 0
// ya que en los bucles for para dibujar las dos mitades del triangulo
// las iteraciones siguen mientras y es mayor que las alturas de los 
// puntos medio e inferior. Por lo tanto, si el superior y el medio o
// el medio y el inferior tienen la misma altura, el primer bucle for
// o el segundo no tendrá ninguna iteración, respectivamente.
void calcular_punto_corte(real_t y, const punto3 *const psupptr, const punto3 *const pinfptr, punto3 *corteptr){
    int cond = psupptr->y > pinfptr->y;
    if(cond == 0){
        printf("PSUP_Y: %f ; PINF_Y: %f\n", psupptr->y, pinfptr->y);
    }
    assert(cond);

    real_t proporcionDifY = (psupptr->y - y) / (psupptr->y - pinfptr->y);      // (*)
    
    cond = 0 <= proporcionDifY && proporcionDifY <= 1;
    if(cond == 0){
        printf("proporcionDifY: %f\n", proporcionDifY);
        printf("y: %f ; ysup: %f ; yinf: %f\n", y, psupptr->y, pinfptr->y);
    }
    assert(cond);
    
    corteptr->x = psupptr->x - proporcionDifY * (psupptr->x - pinfptr->x);
    corteptr->y = y;
    corteptr->z = psupptr->z - proporcionDifY * (psupptr->z - pinfptr->z);
}

void calcular_punto_corte_iluminado(
    real_t y, 
    const punto3 *const psupptr, const punto3 *const pinfptr, 
    const color3 *const Isupptr, const color3 *const Iinfptr,
    punto3 *const pcorteptr, color3 *const Icorteptr)
{
    real_t proporcionDifY = (psupptr->y - y) / (psupptr->y - pinfptr->y);
        
    pcorteptr->x = psupptr->x - proporcionDifY * (psupptr->x - pinfptr->x);
    pcorteptr->y = y;
    pcorteptr->z = psupptr->z - proporcionDifY * (psupptr->z - pinfptr->z);
    
    Icorteptr->r = Isupptr->r - proporcionDifY * (Isupptr->r - Iinfptr->r);
    Icorteptr->g = Isupptr->g - proporcionDifY * (Isupptr->g - Iinfptr->g);
    Icorteptr->b = Isupptr->b - proporcionDifY * (Isupptr->b - Iinfptr->b);
}

void calcular_punto_izq_der(
    punto3 **const pizqptrptr, punto3 **const pderptrptr, 
    punto3 *const p1ptr, punto3 *const p2ptr)
{
    if(p1ptr->x < p2ptr->x){
        *pizqptrptr = p1ptr;
        *pderptrptr = p2ptr;
    } else {
        *pizqptrptr = p2ptr;
        *pderptrptr = p1ptr;
    }
}

void calcular_punto_izq_der_3p(
    punto3 **const pizqptrptr, punto3 **const pderptrptr, 
    punto3 *const p1ptr, punto3 *const p2ptr, punto3 *const p3ptr)
{
    calcular_punto_izq_der(pizqptrptr, pderptrptr, p1ptr, p2ptr);
    if(p3ptr->x < (*pizqptrptr)->x){
        *pizqptrptr = p3ptr;
    } else if(p3ptr->x > (*pderptrptr)->x){
        *pderptrptr = p3ptr;
    }
}

void calcular_punto_izq_der_iluminados(
    punto3 **const pizqptrptr, punto3 **const pderptrptr,
    color3 **const Iizqptrptr, color3 **const Iderptrptr, 
    punto3 *const p1ptr, punto3 *const p2ptr,
    color3 *const I1ptr, color3 *const I2ptr)
{
    if(p1ptr->x < p2ptr->x){
        *pizqptrptr = p1ptr; *Iizqptrptr = I1ptr;
        *pderptrptr = p2ptr; *Iderptrptr = I2ptr;
    } else {
        *pizqptrptr = p2ptr; *Iizqptrptr = I2ptr;
        *pderptrptr = p1ptr; *Iderptrptr = I1ptr;
    }
}

void calcular_punto_izq_der_3p_iluminados(
    punto3 **const pizqptrptr, punto3 **const pderptrptr,
    color3 **const Iizqptrptr, color3 **const Iderptrptr, 
    punto3 *const p1ptr, punto3 *const p2ptr, punto3 *const p3ptr,
    color3 *const I1ptr, color3 *const I2ptr, color3 *const I3ptr)
{
    calcular_punto_izq_der_iluminados(
        pizqptrptr, pderptrptr, Iizqptrptr, Iderptrptr,
        p1ptr, p2ptr, I1ptr, I2ptr);

    if(p3ptr->x < (*pizqptrptr)->x){
        *pizqptrptr = p3ptr; *Iizqptrptr = I3ptr;
    } else if(p3ptr->x > (*pderptrptr)->x){
        *pderptrptr = p3ptr; *Iderptrptr = I3ptr;
    }
}

void calcular_punto_sup_med_inf(
    punto3 **const pgoiptrptr, punto3 **const perdiptrptr, punto3 **const pbeheptrptr,
    punto3 *const p1ptr, punto3 *const p2ptr, punto3 *const p3ptr)
{
    if(p1ptr->y > p2ptr->y){
        *pgoiptrptr = p1ptr;
        *pbeheptrptr = p2ptr;
    } else {
        *pgoiptrptr = p2ptr;
        *pbeheptrptr = p1ptr;
    }
    if(p3ptr->y > (*pgoiptrptr)->y){
        *perdiptrptr = *pgoiptrptr;
        *pgoiptrptr = p3ptr;
    } else {
        if(p3ptr->y > (*pbeheptrptr)->y){
            *perdiptrptr = p3ptr;
        } else {
            *perdiptrptr = *pbeheptrptr;
            *pbeheptrptr = p3ptr;
        }
    }
}

void calcular_punto_sup_med_inf_iluminados(
    punto3 **const pgoiptrptr, punto3 **const perdiptrptr, punto3 **const pbeheptrptr,
    color3 **const Igoiptrptr, color3 **const Ierdiptrptr, color3 **const Ibeheptrptr,
    punto3 *const p1ptr, punto3 *const p2ptr, punto3 *const p3ptr,
    color3 *const I1ptr, color3 *const I2ptr, color3 *const I3ptr)
{
    if(p1ptr->y > p2ptr->y){
        *pgoiptrptr = p1ptr; *Igoiptrptr = I1ptr;
        *pbeheptrptr = p2ptr; *Ibeheptrptr = I2ptr;
    } else {
        *pgoiptrptr = p2ptr; *Igoiptrptr = I2ptr;
        *pbeheptrptr = p1ptr; *Ibeheptrptr = I1ptr;
    }
    if(p3ptr->y > (*pgoiptrptr)->y){
        *perdiptrptr = *pgoiptrptr; *Ierdiptrptr = *Igoiptrptr;
        *pgoiptrptr = p3ptr; *Igoiptrptr = I3ptr;
    } else {
        if(p3ptr->y > (*pbeheptrptr)->y){
            *perdiptrptr = p3ptr; *Ierdiptrptr = I3ptr;
        } else {
            *perdiptrptr = *pbeheptrptr; *Ierdiptrptr = *Ibeheptrptr;
            *pbeheptrptr = p3ptr; *Ibeheptrptr = I3ptr;
        }
    }
}

//
// FUNCIÓN PARA DIBUJAR EL PERÍMETRO DEL TRIANGULO
// Precondición: p1, p2, p3 están en el SRCamara
void dibujar_perimetro_triangulo(const punto3 p1, const punto3 p2, const punto3 p3, const color3 rgb){
    //punto ndc_p1, ndc_p2, ndc_p3;
    //calculateNDCp3(&ndc_p1, p1, &ndc_p2, p2, &ndc_p3, p3);
    
    glColor3ub(rgb.r, rgb.g, rgb.b);
    glBegin(GL_POLYGON);
    //    glVertex3d(ndc_p1.x, ndc_p1.y, ndc_p1.z);
    //    glVertex3d(ndc_p2.x, ndc_p2.y, ndc_p2.z);
    //    glVertex3d(ndc_p3.x, ndc_p3.y, ndc_p3.z);
        glVertex3d(p1.x, p1.y, p1.z);
        glVertex3d(p2.x, p2.y, p2.z);
        glVertex3d(p3.x, p3.y, p3.z);
    glEnd();
}

//
// FUNCIÓN AUXILIAR PARA DIBUJAR EL ÁREA DEL TRIANGULO
//
void dibujar_area_triangulo_color(
    const punto3 *const pgoiptr, 
    punto3 *const perdiptr, 
    punto3 *const pbeheptr, 
    const color3 rgb)
{
    real_t y;
    punto3 c1, c2;
    punto3 *pizqptr, *pderptr;
    
    // Dibujar todo el área del triangulo
    // Primera mitad del triangulo: del punto superior al medio
    for(y = pgoiptr->y; y > perdiptr->y; y -= YCHANGE){
        calcular_punto_corte(y, pgoiptr, pbeheptr, &c1);
        calcular_punto_corte(y, pgoiptr, perdiptr, &c2);
        calcular_punto_izq_der(&pizqptr, &pderptr, &c1, &c2);
        dibujar_linea_z_color(y, pizqptr->x, pizqptr->z, pderptr->x, pderptr->z, rgb);
    }
    // Segunda mitad del triangulo: del punto medio al punto inferior
    for(; y > pbeheptr->y; y -= YCHANGE){
        calcular_punto_corte(y, pgoiptr, pbeheptr, &c1);
        calcular_punto_corte(y, perdiptr, pbeheptr, &c2);
        calcular_punto_izq_der(&pizqptr, &pderptr, &c1, &c2);
        dibujar_linea_z_color(y, pizqptr->x, pizqptr->z, pderptr->x, pderptr->z, rgb);
    }

    // Especialmente para el caso límite donde el punto medio y el inferior
    // estén a la misma altura es necesario para dibujar los puntos 
    // (sino faltaría la recta inferior). Para los demás casos, solo faltaría
    // un único punto inferior.
    if(perdiptr->y == pbeheptr->y){ //== y
        calcular_punto_izq_der(&pizqptr, &pderptr, perdiptr, pbeheptr);
        dibujar_linea_z_color(y, pizqptr->x, pizqptr->z, pderptr->x, pderptr->z, rgb);
    
    } else {    // un único punto, del punto inferior a si mismo
        dibujar_linea_z_color(y, pbeheptr->x, pbeheptr->z, pbeheptr->x, pbeheptr->z, rgb);  
    }
}
// VERSIÓN ILUMINACIÓN:
void dibujar_area_triangulo_iluminado(
    punto3 *const pgoiptr, punto3 *const perdiptr, punto3 *const pbeheptr,
    color3 *const Igoiptr, color3 *const Ierdiptr, color3 *const Ibeheptr)
{
    real_t y;
    punto3 c1, c2;
    color3 I1, I2;
    punto3 *pizqptr, *pderptr;
    color3 *Iizqptr, *Iderptr;

    // Dibujar todo el área del triangulo    
    // Primera mitad del triangulo: del punto superior al medio
    for(y = pgoiptr->y; y > perdiptr->y; y -= YCHANGE){
        calcular_punto_corte_iluminado(y, pgoiptr, pbeheptr, Igoiptr, Ibeheptr, &c1, &I1);
        calcular_punto_corte_iluminado(y, pgoiptr, perdiptr, Igoiptr, Ierdiptr, &c2, &I2);
        calcular_punto_izq_der_iluminados(&pizqptr, &pderptr, &Iizqptr, &Iderptr,
                                          &c1, &c2, &I1, &I2);
        dibujar_linea_z_iluminado(y, pizqptr->x, pizqptr->z, pderptr->x, pderptr->z, *Iizqptr, *Iderptr);
    }
    // Segunda mitad del triangulo: del punto medio al punto inferior
    for(; y > pbeheptr->y; y -= YCHANGE){
        calcular_punto_corte_iluminado(y, pgoiptr, pbeheptr, Igoiptr, Ibeheptr, &c1, &I1);
        calcular_punto_corte_iluminado(y, perdiptr, pbeheptr, Ierdiptr, Ibeheptr, &c2, &I2);
        calcular_punto_izq_der_iluminados(&pizqptr, &pderptr, &Iizqptr, &Iderptr, 
                                          &c1, &c2, &I1, &I2);
        dibujar_linea_z_iluminado(y, pizqptr->x, pizqptr->z, pderptr->x, pderptr->z, *Iizqptr, *Iderptr);
    }
    // Especialmente para el caso límite donde el punto medio y el inferior
    // estén a la misma altura es necesario para dibujar los puntos 
    // (sino faltaría la recta inferior). Para los demás casos, solo faltaría
    // el punto inferior.
    if(perdiptr->y == pbeheptr->y){ //== y
        calcular_punto_izq_der_iluminados(&pizqptr, &pderptr, &Iizqptr, &Iderptr,
                                          perdiptr, pbeheptr, Ierdiptr, Ibeheptr);
        dibujar_linea_z_iluminado(y, pizqptr->x, pizqptr->z, pderptr->x, pderptr->z, *Iizqptr, *Iderptr);

    } else {    // un único punto, del punto inferior a si mismo
        dibujar_linea_z_iluminado(y, pbeheptr->x, pbeheptr->z, pbeheptr->x, pbeheptr->z, *Ibeheptr, *Ibeheptr);
    }
}


//
// FUNCIÓN AUXILIAR PARA DIBUJAR UNA LÍNEA HORIZONTAL
// * Precondición: no tiene mucho sentido usar esta función a menos que p1, p2 y p3 estén alineados
//
void dibujar_linea_horizontal_iluminado(
    punto3 p1, punto3 p2, punto3 p3,
    color3 I1, color3 I2, color3 I3)
{
    punto3 *pizqptr, *pderptr;
    color3 *Iizqptr, *Iderptr;
    
    calcular_punto_izq_der_3p_iluminados(
        &pizqptr, &pderptr, &Iizqptr, &Iderptr,
        &p1, &p2, &p3,
        &I1, &I2, &I3);
    dibujar_linea_z_iluminado(p1.y, pizqptr->x, pizqptr->z, pderptr->x, pderptr->z, *Iizqptr, *Iderptr);
}

void dibujar_linea_horizontal_color(punto3 p1, punto3 p2, punto3 p3, const color3 rgb){
    punto3 *pizqptr, *pderptr;
    
    calcular_punto_izq_der_3p(&pizqptr, &pderptr, &p1, &p2, &p3);
    dibujar_linea_z_color(p1.y, pizqptr->x, pizqptr->z, pderptr->x, pderptr->z, rgb);
}

//
// FUNCIONES QUE DIBUJAN LOS TRIÁNGULOS
//
void dibujar_triangulo_iluminado(punto3 p1, punto3 p2, punto3 p3, color3 I1, color3 I2, color3 I3){
    punto3 *pgoiptr, *pbeheptr, *perdiptr;
    color3 *Igoiptr, *Ibeheptr, *Ierdiptr;

    // A partir de aquí los puntos se transforman a NDC (normalized device coordinates)
    // dentro del bucle de dibujar_linea_z.c

    // CASOS ESPECIALES
    // Los tres puntos en la misma altura dibujar linea del punto más a la izquierda al más a la derecha
    // Caso concreto, dos de los tres puntos son el mismo
    // Caso concreto, los tres puntos son el mismo
    if(p1.y == p2.y && p2.y == p3.y){
        dibujar_linea_horizontal_iluminado(p1, p2, p3, I1, I2, I3);
        return;
    }

    calcular_punto_sup_med_inf_iluminados(
        &pgoiptr, &perdiptr, &pbeheptr,
        &Igoiptr, &Ierdiptr, &Ibeheptr, 
        &p1, &p2, &p3,
        &I1, &I2, &I3);

    assert((pgoiptr->y >= perdiptr->y) && (perdiptr->y >= pbeheptr->y));
    assert(isfinite(pgoiptr->y) && isfinite(perdiptr->y) && isfinite(pbeheptr->y));

    dibujar_area_triangulo_iluminado(pgoiptr, perdiptr, pbeheptr, Igoiptr, Ierdiptr, Ibeheptr);
}

void dibujar_triangulo_color(punto3 p1, punto3 p2, punto3 p3, const color3 rgb){
    punto3 *pgoiptr, *pbeheptr, *perdiptr;

    // A partir de aquí los puntos se transforman a NDC (normalized device coordinates)
    // dentro del bucle de dibujar_linea_z.c

    // CASOS ESPECIALES
    // Los tres puntos en la misma altura dibujar linea del punto más a la izquierda al más a la derecha
    // Caso concreto, dos de los tres puntos son el mismo
    // Caso concreto, los tres puntos son el mismo
    if(p1.y == p2.y && p2.y == p3.y){
        dibujar_linea_horizontal_color(p1, p2, p3, rgb);
        return;
    }

    calcular_punto_sup_med_inf(&pgoiptr, &perdiptr, &pbeheptr, &p1, &p2, &p3);
    assert((pgoiptr->y >= perdiptr->y) && (perdiptr->y >= pbeheptr->y));
    assert(isfinite(pgoiptr->y) && isfinite(perdiptr->y) && isfinite(pbeheptr->y));

    dibujar_area_triangulo_color(pgoiptr, perdiptr, pbeheptr, rgb);
}
