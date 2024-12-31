#ifndef OBJ_H
#define	OBJ_H

//Tipo para numeros reales (aritmetica en coma flotante)
typedef float real_t;
//Necesario para printf y scanf (y similares)
#define REAL_T_SPECIFIER "%f" // "%f" (float) - "%lf" (double)
//Tipo booleano por legibilidad
typedef unsigned char bool;

/** STRUCTURES **/

/****************************
 * Structure to store the   *
 * coordinates and texture coordinates of 3D points *
 ****************************/
typedef struct punto
{
    real_t x, y, z, u,v;
} punto;


/****************************
 * Structure to store the   *
 * coordinates of 3D points *
 ****************************/
typedef struct {
    real_t x, y, z;
} punto3;

/*****************************
 * Structure to store the    *
 * coordinates of 3D vectors *
 *****************************/
typedef struct {
    real_t x, y, z;
} vector;

/****************************
 * Structure to store the   *
 * colors in RGB mode       *
 ****************************/
typedef struct {
    real_t r, g, b;
} color3;

/****************************
 * Structure to store       *
 * the list of matices      *
 ****************************/

typedef struct mlist
{
    real_t m[16];
    struct mlist *hptr;
} mlist;


/***************************
 * Light
 ***************************/
 
typedef enum { ENVIRONMENT, DIRECTIONAL, POSITIONAL, SPOT } LightType; 

typedef struct light
{
    bool onoff;
    LightType type;     // 0 -> directional, 1 -> positional, 2 -> spot light
    color3 I;
    punto3 pos;         // positional or spot light
    punto3 campos;
    vector dir;         // directional or spot light
    vector camdir;
    real_t aperture_cos;    // cos(ang) if  0 --> any position is iluminated. 
                            // if (not 0) only the cone is iluminated.
                        
    mlist *mptr;        // lista de transformaciones para el undo (solo para directional o positional)
} light;


/****************************
 * Structure to store       *
 * objects' vertices        *
 ****************************/
typedef struct {
    punto3 coord;                       /* coordinates x, y, z */
    //vector normal del vertice == suma (normalizada) de los vectores normales de las caras que comparten el vertice
    punto3 N;
} vertex;

/****************************
 * Structure to store       *
 * objects' faces or        *
 * polygons                 *
 ****************************/
typedef struct {
    int num_vertices;                 /* number of vertices in the face */
    int *vertex_ind_table;            /* table with the index of each vertex */
    punto3 center;
    punto3 N;
} face;


/****************************
 * Structure to store a     *
 * pile of 3D objects       *
 ****************************/
struct object3d{
    int num_vertices;                       /* number of vertices in the object*/
    vertex *vertex_table;                   /* table of vertices */
    int num_faces;                          /* number of faces in the object */
    face *face_table;                       /* table of faces */

    punto3 center;                          //el centro del objeto (para el lookAt)
    unsigned int num_transf_analisis_mode;  //cuando vale 0 volvemos al modo VUELO; solo relevante en camaras
    
    mlist *mptr;
    color3 rgb;
    color3 ka;
    color3 kd;
    color3 ks;
    real_t ns;
    struct object3d *hptr;              /* next element in the pile of objects */
};

typedef struct object3d object3d;
typedef object3d object;
typedef object camara;

#endif	/* OBJ_H */
