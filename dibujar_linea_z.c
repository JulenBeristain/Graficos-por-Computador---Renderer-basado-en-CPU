#include <GL/glut.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "dibujar_linea_z.h"
#include "lights.h"
#include "matrix_operations.h"
//#include "camaras.h"

#define XCHANGE 0.1

//interpolación Gouraud
void  dibujar_linea_z_iluminado(
    real_t linea, real_t c1x, real_t c1z, real_t c2x, real_t c2z,
    const color3 I1, const color3 I2)
{
    real_t xkoord, zkoord;
    color3 I;
    //punto3 ndcp, p;

    const real_t difx = (c2x - c1x) / XCHANGE;
    const real_t zchange = (c2z - c1z) / difx;
    const color3 Ichange = (color3){(I2.r - I1.r)/difx, (I2.g - I1.g)/difx, (I2.b - I1.b)/difx};

    glBegin( GL_POINTS );
    for (xkoord = c1x, zkoord = c1z, I = I1; xkoord <= c2x; xkoord += XCHANGE){
        //Clamping: valor máximo para los componentes rgb == 255
        I.r = fmin(I.r, 255); I.g = fmin(I.g, 255); I.b = fmin(I.b, 255);
        glColor3ub(I.r, I.g, I.b);

        //p = (punto3){xkoord, linea, zkoord};
        //calculateNDCp1(&ndcp, p);
        //glVertex3f(ndcp.x, ndcp.y, ndcp.z);
        glVertex3f(xkoord, linea, zkoord);
        
        zkoord += zchange;
        I = (color3){I.r + Ichange.r, I.g + Ichange.g, I.b + Ichange.b};
    }
    glEnd();
}

void  dibujar_linea_z_color(real_t linea, real_t c1x, real_t c1z, real_t c2x, real_t c2z, const color3 rgb)
{
    real_t xkoord, zkoord;
    //punto3 ndcp, p;

    int cond = isfinite(c1x) && isfinite(c2x);
    if(cond == 0){
        printf("c2x: %f", c2x);
        printf(" - c1x: %f", c1x);
    }
    assert(cond);

    real_t difx = (c2x - c1x) / XCHANGE;
    real_t zchange = (c2z-c1z) / difx;

    cond = isfinite(difx); // && isfinite(zchange)
    if(cond == 0){
        printf(" - difx: %f", difx);
    }
    assert(cond);

    glBegin( GL_POINTS );
    glColor3ub(rgb.r, rgb.g, rgb.b);
    for (xkoord = c1x, zkoord = c1z; xkoord <= c2x; xkoord += XCHANGE){
        //p = (punto3){xkoord, linea, zkoord};
        //calculateNDCp1(&ndcp, p);
        //glVertex3f(ndcp.x, ndcp.y, ndcp.z);
        glVertex3f(xkoord, linea, zkoord);

        // calcular zkoord del siguiente pixel        
        zkoord += zchange;
    }
    glEnd();
}

