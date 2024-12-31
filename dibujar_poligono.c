#include <GL/glut.h>
#include <assert.h>
#include <stdio.h>
#include "dibujar_poligono.h"
#include "dibujar_triangulo.h"
#include "dibujar_linea_z.h"
#include "camaras.h"
#include "matrix_operations.h"
#include "lights.h"

//
// FUNCIÓN DE UTILIDAD PARA ENCAPSULAR LA LLAMADA A OPENGL PARA DIBUJAR UNA LÍNEA
//
void dibujar_linea(const punto3 p1, const punto3 p2, const color3 rgb){
    glColor3ub(rgb.r, rgb.g, rgb.b);
    glBegin(GL_LINES);
        glVertex3d(p1.x, p1.y, p1.z);
        glVertex3d(p2.x, p2.y, p2.z);
    glEnd();
}

//Precondicion: p1 y p2 ya están en el SRObservador
void mostrar_normal_cara(const punto3 p1, const punto3 p2, const color3 rgb){
    dibujar_linea(p1, p2, rgb);
}
// FUNCIÓN QUE NO HACE NADA PARA PODER PARAMETRIZAR LAS FUNCIONES QUE MUESTRAN NORMALES
void no_mostrar_normal_cara(const punto3 p1, const punto3 p2, const color3 rgb){}

//Precondición: p_sro en SRObservador pero pN no
void mostrar_normal_vertice(const punto3 p1, const punto3 p2, const color3 rgb){
    dibujar_linea(p1, p2, rgb);
}
void no_mostrar_normal_vertice(const punto3 p1, const punto3 p2, const color3 rgb){}

// VARIABLES GLOBALES PARA SELECCIONAR LA FUNCIÓN A EMPLEAR PARA DIBUJAR VECTORES NORMALES:
// Es decir, dibujarlos o no
funcMostrarNormalCara f_mostrar_normal_cara;
funcMostrarNormalVertice f_mostrar_normal_vertice;


//
// FUNCIÓN AUXILIAR PARA DECIDIR SI EL POLIGONO ES TRASERO; es decir, NO ESTÁ DE CARA A LA DIRECCIÓN DE PROYECCIÓN
// Precondición: center y pnormal en el SRCámara
bool is_looking_backwards_paralelo(const punto3 center, const vector normal){
    return normal.z < 0;
}

bool is_looking_backwards_perspectiva(const punto3 center, const vector normal){
    vector v_proy;

    //los puntos ya están en el sistema de referencia de la cámara (m_dibujo == mcsr * ...)
    //por lo tanto, la dirección de proyección es el vector: center --> O(0,0,0)
    v_proy = normalized(calculate_vector_AtoB3(center, (punto3){0,0,0}));
    return scalar_product(normal, v_proy) < 0;
}

// VARIABLE GLOBAL PARA SELECCIONAR LA FUNCIÓN A EMPLEAR PARA COMPROBAR QUE LA CARA ES TRASERA
funcTestBackwards f_is_looking_backwards;


//FUNCIÓN AUXILIAR PARA DIBUJAR VECTORES NORMALES CADA VEZ QUE CALCULO LA POSICIÓN DE UN VÉRTICE
void mxp3_mostrar_normal(
    punto3 *const pptr, const vertex *const vptr, 
    const real_t *const mcsr_transf, const color3 rgb)
{
    punto3 pnormal;
    
    mxp3(pptr, mcsr_transf, vptr->coord);
    mxp3(&pnormal, mcsr_transf, vptr->N);
    f_mostrar_normal_vertice(*pptr, pnormal, rgb);
}

//FUNCIÓN AUXILIAR PARA QUE ADEMÁS DE LO ANTERIOR TAMBIÉN SE CALCULE EL VECTOR NORMAL (no solo mostrarlo)
void mxp3_mostrar_calcular_normal(
    punto3 *const pptr, vector *const Niptr, const vertex *const vptr, 
    const real_t *const mcsr_transf, const color3 rgb)
{
    punto3 pnormal;

    mxp3(pptr, mcsr_transf, vptr->coord);
    mxp3(&pnormal, mcsr_transf, vptr->N);
    f_mostrar_normal_vertice(*pptr, pnormal, rgb);
    *Niptr = normalized(calculate_vector_AtoB3(*pptr, pnormal));
}

// TABLA DE VARIABLES GLOBALES BOOLEANAS QUE DEFINEN QUE FUNCIÓN DE DIBUJADO DE POLIGONO UTILIZAR
//
// take_normals_of --> 0 == FACES // 1 == VERTICES
//
// mostrar_caras traseras | lineak | take_normals_of || Índice de la función
//            0                0            0                    0
//            0                0            1                    1
//            0                1            0                    2
//            0                1            1                    3
//            1                0            0                    4
//            1                0            1                    5
//            1                1            0                    6
//            1                1            1                    7
//
// Cada una de las combinaciones es una función: 2^3 == 8 combinaciones --> 8 funciones
// No es muy extensible, es el precio a pagar por utilizar variables booleanas para guardar el estado
// de la aplicación. Si quisieramos meter una variable booleana extra que afecta a cómo dibujamos los
// objetos, la cantidad de las funciones potencialmente se volvería a duplicar (a menos que podamos utilizar
// estrategias alternativas como pasar por parámetro, parecido a como lo hemos hecho con mostrar_vectores_normales).
//
// La razón por la que nos metemos en estas complicaciones es porque no tiene sentido andar comprobando
// en las condicionales para cada poligono valores de variables globales booleanas cuyos valores no cambian
// y que son conocidas ya al principio de la función marraztu. Podemos seleccionar ya en ese momento la función
// que vamos a utilizar para dibujar los poligonos.

// IMPLEMENTACIÓN DE CADA FUNCIÓN
// NO mostrar_caras_traseras - dibujar AREAS triangulos - take_normals_of FACES
void dibujar_poligono_0(
    const object *const optr, const face *const fptr, 
    const real_t *const mcsr_transf, const real_t *const mcsr)
{
    unsigned int i;
    vertex *v1, *v2, *v3;
    bool es_trasera;
    punto3 center, pnormal, p1, p2, p3;
    vector N;
    color3 I1, I2, I3;

    //transformamos y pasamos el centro de la cara y el punto del vector normal en SRLocal del objeto al SRCamara
    mxp3(&center, mcsr_transf, fptr->center);
    mxp3(&pnormal, mcsr_transf, fptr->N);

    //calculamos el vector normal de la cara
    N = normalized(calculate_vector_AtoB3(center, pnormal));

    //calculamos si la cara es trasera
    es_trasera = f_is_looking_backwards(center, N);
    if(es_trasera){
        return;
    }
    
    //dibujar vectores normales del poligono (face)
    //punto ndc_p1, ndc_pnormal;
    //calculateNDCp2(&ndc_p1, p1, &ndc_pnormal, pnormal);
    f_mostrar_normal_cara(center, pnormal, optr->rgb);

    //cada cara tiene como minimo tres vertices
    v1 = optr->vertex_table + fptr->vertex_ind_table[0];
    v2 = optr->vertex_table + fptr->vertex_ind_table[1];
    v3 = optr->vertex_table + fptr->vertex_ind_table[2];
    mxp3_mostrar_normal(&p1, v1, mcsr_transf, optr->rgb);
    I1 = calculate_intensity(p1, optr, N);
    mxp3_mostrar_normal(&p2, v2, mcsr_transf, optr->rgb);
    I2 = calculate_intensity(p2, optr, N);

    for(i = 3; ; ++i){
        mxp3_mostrar_normal(&p3, v3, mcsr_transf, optr->rgb);
        I3 = calculate_intensity(p3, optr, N);
        dibujar_triangulo_iluminado(p1, p2, p3, I1, I2, I3);
        if(i == fptr->num_vertices) break;
        p2 = p3;
        I2 = I3;
        v3 = optr->vertex_table + fptr->vertex_ind_table[i];
    }
}

// NO mostrar_caras_traseras - dibujar AREAS triangulos - take_normals_of VERTICES
void dibujar_poligono_1(
    const object *const optr, const face *const fptr, 
    const real_t *const mcsr_transf, const real_t *const mcsr)
{
    unsigned int i;
    vertex *v1, *v2, *v3;
    bool es_trasera;
    punto3 center, pnormal, p1, p2, p3;
    vector N, N1, N2, N3;
    color3 I1, I2, I3;

    //transformamos y pasamos el centro de la cara y el punto del vector normal en SRLocal del objeto al SRCamara
    mxp3(&center, mcsr_transf, fptr->center);
    mxp3(&pnormal, mcsr_transf, fptr->N);

    //calculamos el vector normal de la cara
    N = normalized(calculate_vector_AtoB3(center, pnormal));

    //calculamos si la cara es trasera
    es_trasera = f_is_looking_backwards(center, N);
    if(es_trasera){
        return;
    }
    
    //dibujar vectores normales del poligono (face)
    //punto ndc_p1, ndc_pnormal;
    //calculateNDCp2(&ndc_p1, p1, &ndc_pnormal, pnormal);
    f_mostrar_normal_cara(center, pnormal, optr->rgb);

    //cada cara tiene como minimo tres vertices
    v1 = optr->vertex_table + fptr->vertex_ind_table[0];
    v2 = optr->vertex_table + fptr->vertex_ind_table[1];
    v3 = optr->vertex_table + fptr->vertex_ind_table[2];
    mxp3_mostrar_calcular_normal(&p1, &N1, v1, mcsr_transf, optr->rgb);
    I1 = calculate_intensity(p1, optr, N1);
    mxp3_mostrar_calcular_normal(&p2, &N2, v2, mcsr_transf, optr->rgb);
    I2 = calculate_intensity(p2, optr, N2);
    for(i = 3; ; ++i){
        mxp3_mostrar_calcular_normal(&p3, &N3, v3, mcsr_transf, optr->rgb);
        I3 = calculate_intensity(p3, optr, N3);
        dibujar_triangulo_iluminado(p1, p2, p3, I1, I2, I3);
        if(i == fptr->num_vertices) break;
        p2 = p3;
        I2 = I3;
        v3 = optr->vertex_table + fptr->vertex_ind_table[i];
    }
}

// NO mostrar_caras_traseras - dibujar PERIMETROS triangulos - take_normals_of FACES
void dibujar_poligono_2(
    const object *const optr, const face *const fptr, 
    const real_t *const mcsr_transf, const real_t *const mcsr)
{
    unsigned int i;
    vertex *v1, *v2, *v3;
    bool es_trasera;
    punto3 center, pnormal, p1, p2, p3;
    vector N;

    //transformamos y pasamos el centro de la cara y el punto del vector normal en SRLocal del objeto al SRCamara
    mxp3(&center, mcsr_transf, fptr->center);
    mxp3(&pnormal, mcsr_transf, fptr->N);

    //calculamos el vector normal de la cara
    N = normalized(calculate_vector_AtoB3(center, pnormal));

    //calculamos si la cara es trasera
    es_trasera = f_is_looking_backwards(center, N);
    if(es_trasera){
        return;
    }
    
    //dibujar vectores normales del poligono (face)
    //punto ndc_p1, ndc_pnormal;
    //calculateNDCp2(&ndc_p1, p1, &ndc_pnormal, pnormal);
    f_mostrar_normal_cara(center, pnormal, optr->rgb);

    //cada cara tiene como minimo tres vertices
    v1 = optr->vertex_table + fptr->vertex_ind_table[0];
    v2 = optr->vertex_table + fptr->vertex_ind_table[1];
    v3 = optr->vertex_table + fptr->vertex_ind_table[2];
    mxp3_mostrar_normal(&p1, v1, mcsr_transf, optr->rgb);
    mxp3_mostrar_normal(&p2, v2, mcsr_transf, optr->rgb);

    for(i = 3; ; ++i){
        mxp3_mostrar_normal(&p3, v3, mcsr_transf, optr->rgb);
        dibujar_perimetro_triangulo(p1, p2 , p3, optr->rgb);
        if(i == fptr->num_vertices) break;
        p2 = p3;
        v3 = optr->vertex_table + fptr->vertex_ind_table[i];
    }
}

// NO mostrar_caras_traseras - dibujar PERIMETROS triangulos - take_normals_of VERTICES
//void dibujar_poligono_3: igual que el dos ya que al dibujar perimetros nos dan igual las normales
#define dibujar_poligono_3 dibujar_poligono_2


// SÍ mostrar_caras_traseras - dibujar AREAS triangulos - take_normals_of FACES
void dibujar_poligono_4(
    const object *const optr, const face *const fptr, 
    const real_t *const mcsr_transf, const real_t *const mcsr)
{
    unsigned int i;
    vertex *v1, *v2, *v3;
    bool es_trasera;
    punto3 center, pnormal, p1, p2, p3;
    vector N;
    color3 rgb = optr->rgb;
    color3 I1, I2, I3;

    //transformamos y pasamos el centro de la cara y el punto del vector normal en SRLocal del objeto al SRCamara
    mxp3(&center, mcsr_transf, fptr->center);
    mxp3(&pnormal, mcsr_transf, fptr->N);

    //calculamos el vector normal de la cara
    N = normalized(calculate_vector_AtoB3(center, pnormal));

    //dibujar vectores normales del poligono (face)
    //punto ndc_p1, ndc_pnormal;
    //calculateNDCp2(&ndc_p1, p1, &ndc_pnormal, pnormal);
    f_mostrar_normal_cara(center, pnormal, rgb);

    //cada cara tiene como minimo tres vertices
    v1 = optr->vertex_table + fptr->vertex_ind_table[0];
    v2 = optr->vertex_table + fptr->vertex_ind_table[1];
    v3 = optr->vertex_table + fptr->vertex_ind_table[2];
    mxp3_mostrar_normal(&p1, v1, mcsr_transf, rgb);
    mxp3_mostrar_normal(&p2, v2, mcsr_transf, rgb);

    //calculamos si la cara es trasera
    es_trasera = f_is_looking_backwards(center, N);
    if(es_trasera){
        rgb = RED;
        for(i = 3; ; ++i){
            mxp3_mostrar_normal(&p3, v3, mcsr_transf, rgb);
            //Vector normal del vértice ya calculado (antes del testeo de is_looking_backwards)
            dibujar_triangulo_color(p1, p2, p3, rgb);
            if(i == fptr->num_vertices) break;
            p2 = p3;
            v3 = optr->vertex_table + fptr->vertex_ind_table[i];
        }
    } else {
        I1 = calculate_intensity(p1, optr, N);
        I2 = calculate_intensity(p2, optr, N);
        for(i = 3; ; ++i){
            mxp3_mostrar_normal(&p3, v3, mcsr_transf, optr->rgb);
            I3 = calculate_intensity(p3, optr, N);
            dibujar_triangulo_iluminado(p1, p2, p3, I1, I2, I3);
            if(i == fptr->num_vertices) break;
            p2 = p3;
            I2 = I3;
            v3 = optr->vertex_table + fptr->vertex_ind_table[i];
        }
    }
}

// SÍ mostrar_caras_traseras - dibujar AREAS triangulos - take_normals_of VERTICES
void dibujar_poligono_5(
    const object *const optr, const face *const fptr, 
    const real_t *const mcsr_transf, const real_t *const mcsr)
{
    unsigned int i;
    vertex *v1, *v2, *v3;
    bool es_trasera;
    punto3 center, pnormal, p1, p2, p3;
    vector N, N1, N2, N3;
    color3 I1, I2, I3;
    color3 rgb = optr->rgb;

    //transformamos y pasamos el centro de la cara y el punto del vector normal en SRLocal del objeto al SRCamara
    mxp3(&center, mcsr_transf, fptr->center);
    mxp3(&pnormal, mcsr_transf, fptr->N);

    //calculamos el vector normal de la cara
    N = normalized(calculate_vector_AtoB3(center, pnormal));

    //dibujar vectores normales del poligono (face)
    //punto ndc_p1, ndc_pnormal;
    //calculateNDCp2(&ndc_p1, p1, &ndc_pnormal, pnormal);
    f_mostrar_normal_cara(center, pnormal, rgb);

    //cada cara tiene como minimo tres vertices
    v1 = optr->vertex_table + fptr->vertex_ind_table[0];
    v2 = optr->vertex_table + fptr->vertex_ind_table[1];
    v3 = optr->vertex_table + fptr->vertex_ind_table[2];
    

    //calculamos si la cara es trasera
    es_trasera = f_is_looking_backwards(center, N);
    if(es_trasera){
        rgb = RED;
        mxp3_mostrar_normal(&p1, v1, mcsr_transf, rgb);
        mxp3_mostrar_normal(&p2, v2, mcsr_transf, rgb);
        for(i = 3; ; ++i){
            mxp3_mostrar_normal(&p3, v3, mcsr_transf, rgb);
            //Vector normal del vértice ya calculado (antes del testeo de is_looking_backwards)
            dibujar_triangulo_color(p1, p2, p3, rgb);
            if(i == fptr->num_vertices) break;
            p2 = p3;
            v3 = optr->vertex_table + fptr->vertex_ind_table[i];
        }
    } else {
        mxp3_mostrar_calcular_normal(&p1, &N1, v1, mcsr_transf, rgb);
        I1 = calculate_intensity(p1, optr, N1);
        mxp3_mostrar_calcular_normal(&p2, &N2, v2, mcsr_transf, rgb);
        I2 = calculate_intensity(p2, optr, N2);
        for(i = 3; ; ++i){
            mxp3_mostrar_calcular_normal(&p3, &N3, v3, mcsr_transf, optr->rgb);
            I3 = calculate_intensity(p3, optr, N3);
            dibujar_triangulo_iluminado(p1, p2, p3, I1, I2, I3);
            if(i == fptr->num_vertices) break;
            p2 = p3;
            I2 = I3;
            v3 = optr->vertex_table + fptr->vertex_ind_table[i];
        }
    }
}

// SÍ mostrar_caras_traseras - dibujar PERIMETROS triangulos - take_normals_of FACES
void dibujar_poligono_6(
    const object *const optr, const face *const fptr, 
    const real_t *const mcsr_transf, const real_t *const mcsr)
{
    unsigned int i;
    vertex *v1, *v2, *v3;
    bool es_trasera;
    punto3 center, pnormal, p1, p2, p3;
    vector N;
    color3 rgb = optr->rgb;

    //transformamos y pasamos el centro de la cara y el punto del vector normal en SRLocal del objeto al SRCamara
    mxp3(&center, mcsr_transf, fptr->center);
    mxp3(&pnormal, mcsr_transf, fptr->N);

    //calculamos el vector normal de la cara
    N = normalized(calculate_vector_AtoB3(center, pnormal));

    //calculamos si la cara es trasera
    es_trasera = f_is_looking_backwards(center, N);
    if(es_trasera){
        rgb = RED;
    }
    
    //dibujar vectores normales del poligono (face)
    //punto ndc_p1, ndc_pnormal;
    //calculateNDCp2(&ndc_p1, p1, &ndc_pnormal, pnormal);
    f_mostrar_normal_cara(center, pnormal, optr->rgb);

    //cada cara tiene como minimo tres vertices
    v1 = optr->vertex_table + fptr->vertex_ind_table[0];
    v2 = optr->vertex_table + fptr->vertex_ind_table[1];
    v3 = optr->vertex_table + fptr->vertex_ind_table[2];
    mxp3_mostrar_normal(&p1, v1, mcsr_transf, optr->rgb);
    mxp3_mostrar_normal(&p2, v2, mcsr_transf, optr->rgb);

    for(i = 3; ; ++i){
        mxp3_mostrar_normal(&p3, v3, mcsr_transf, optr->rgb);
        dibujar_perimetro_triangulo(p1, p2 , p3, optr->rgb);
        if(i == fptr->num_vertices) break;
        p2 = p3;
        v3 = optr->vertex_table + fptr->vertex_ind_table[i];
    }
}

// SÍ mostrar_caras_traseras - dibujar PERIMETROS triangulos - take_normals_of VERTICES
//void dibujar_poligono_7: igual que el seis ya que al dibujar perimetros nos dan igual las normales
#define dibujar_poligono_7 dibujar_poligono_6

// Array con las direcciones de memoria de las funciones para dibujar poligonos
const funcDibujarPoligono tabla_dibujar_poligono[] = {
    dibujar_poligono_0,
    dibujar_poligono_1,
    dibujar_poligono_2,
    dibujar_poligono_3,
    dibujar_poligono_4,
    dibujar_poligono_5,
    dibujar_poligono_6,
    dibujar_poligono_7
};

// Los "pesos de los bits" correspondientes a cada variable
#define WEIGHT_MOSTRAR_CARAS_TRASERAS   2
#define WEIGHT_LINEAK                   1
#define WEIGHT_TAKE_NORMALS_OF          0

// Funcion de elección del índice correcto O define de la expresion con los nombres de las variables globales
// O funcion que utilice las variables globales

// Variables globales externas utilizadas (take_normals_of en ligths.h)
extern bool mostrar_caras_traseras;
extern bool lineak;

unsigned int indice_dibujar_poligono(){
    return (mostrar_caras_traseras << WEIGHT_MOSTRAR_CARAS_TRASERAS) +
           (lineak                 << WEIGHT_LINEAK                ) +
           (take_normals_of        << WEIGHT_TAKE_NORMALS_OF       ) ;
}