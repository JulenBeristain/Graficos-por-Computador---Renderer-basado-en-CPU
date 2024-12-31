#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "obj.h"
#include "matrix_operations.h"

#define MAXLINE     200
#define NORMAL_SC   5

/*
 * Auxiliar function to process each line of the file
 */
static int sreadint(char * lerroa, int * zenbakiak) {
    char *s = lerroa;
    int i, zbk, kont = 0;

    while (sscanf(s, " %d%n", &zbk, &i) > 0) {
        s += i;
        zenbakiak[kont++] = zbk;
    }
    return (kont);
}

static int sreadint2(char * lerroa, int * zenbakiak) {
    char *s = lerroa;
    int i, zbk, kont = 0;

    while (sscanf(s, " %d%n", &zbk, &i) > 0) {
        s += i;
	    while ((*s != ' ')&&(*s !='\0')) s++;  // jump vector normal information
        zenbakiak[kont++] = zbk;
    }
    //printf("%d numbers in the line\n",kont);
    return (kont);
}

/**
 * @brief Function to read wavefront files (*.obj)
 * @param file_name Path of the file to be read
 * @param object_ptr Pointer of the object3d type structure where the data will be stored
 * @return Result of the reading: 0=Read ok, 1=File not found, 2=Invalid file, 3=Empty file
 */
int read_wavefront(const char *const file_name, object3d * object_ptr) {
    vertex *vertex_table;
    face *face_table;
    int num_vertices = -1, num_faces = -1, count_vertices = 0, count_faces = 0;
    FILE *obj_file;
    char line[MAXLINE], line_1[MAXLINE], aux[45];
    int k;
    int i, j;
    int r, g, b;
    int values[MAXLINE];


    /*
     * The function reads twice the file. In the first read the number of
     * vertices and faces is obtained. Then, memory is allocated for each
     * of them and in the second read the actual information is read and
     * loaded. Finally, the object structure is created
     */
    if ((obj_file = fopen(file_name, "r")) == NULL) return (1);
    while (fscanf(obj_file, "\n%[^\n]", line) > 0) 
        {
        i = 0;
        while (line[i] == ' ') i++;
        if ((line[0] == '#') && ((int) strlen(line) > 5)) 
            {
            i += 2;
            j = 0;
            // it is posible a line of the form "# number vertices" where "number" is a number
            // it is posible a line of the form "# number elements" where "number" is a number
            // it is posible a line of the form "# color r g b" where "r", "g" and "b" are numbers <256
            while (line[i] != ' ') line_1[j++] = line[i++];
            i++;
            line_1[j] = '\0';
            j = 0;
            if ((strcmp(line_1, "color") == 0) || (strcmp(line_1, "colour") == 0))
                {
                k = sscanf(line + i, "%d%d%d",&r,&g,&b);
                if (k ==3) 
                    {
                    object_ptr->rgb.r = r;
                    object_ptr->rgb.g = g;
                    object_ptr->rgb.b = b;
                    }
                }
              else
                {
                while ((line[i] != ' ') && (line[i] != '\0'))
                    aux[j++] = line[i++];
                aux[j] = 0;
                if (strcmp(aux, "vertices") == 0)  num_vertices = atoi(line_1);
                if (strncmp(aux, "elements", 7) == 0) num_faces = atoi(line_1);
                }
            } 
          else 
            {
            if (strlen(line) > 6)
                {
                if (line[i] == 'f' && line[i + 1] == ' ')
                    count_faces++;
                else if (line[i] == 'v' && line[i + 1] == ' ')
                    count_vertices++;
                }
            }
        }
    fclose(obj_file);
printf("1 pasada: num vert = %d (%d), num faces = %d(%d) \n",num_vertices,count_vertices,num_faces,count_faces);
    if ((num_vertices != -1 && num_vertices != count_vertices) || (num_faces != -1 && num_faces != count_faces)) {
        printf("WARNING: full file format: (%s)\n", file_name);
        //return (2);
    }
    if (num_vertices == 0 || count_vertices == 0) {
        printf("No vertex found: (%s)\n", file_name);
        return (3);
    }
    if (num_faces == 0 || count_faces == 0) {
        printf("No faces found: (%s)\n", file_name);
        return (3);
    }

    num_vertices = count_vertices;
    num_faces = count_faces;

    vertex_table = (vertex *) malloc(num_vertices * sizeof (vertex));
    face_table = (face *) malloc(num_faces * sizeof (face));

    obj_file = fopen(file_name, "r");
    k = 0;
    j = 0;

    while (fscanf(obj_file, "\n%[^\n]", line) > 0) {
        switch (line[0]) {
            case 'v':
            if (line[1] == ' ')  // vn not interested
		{
                sscanf(line + 2, REAL_T_SPECIFIER REAL_T_SPECIFIER REAL_T_SPECIFIER, &(vertex_table[k].coord.x),
                        &(vertex_table[k].coord.y), &(vertex_table[k].coord.z));
                k++;
		}
               break;

            case 'f':
	    if (line[1] == ' ') // fn not interested
                {
                for (i = 2; i <= (int) strlen(line); i++)
                    line_1[i - 2] = line[i];
		line_1[i-2] = '\0';
                face_table[j].num_vertices = sreadint2(line_1, values);
//printf("f %d vertices\n",face_table[j].num_vertices);
                face_table[j].vertex_ind_table = (int *) malloc(face_table[j].num_vertices * sizeof (int));
                for (i = 0; i < face_table[j].num_vertices; i++) {
                    face_table[j].vertex_ind_table[i] = values[i] - 1;
//printf(" %d ",values[i] - 1);
                    }
//printf("\n");
                j++;
                }
              break;
        }
    }

    fclose(obj_file);

printf("2 pasada\n");

    /*
     * Information read is introduced in the structure */
    object_ptr->vertex_table = vertex_table;
    object_ptr->face_table = face_table;
    object_ptr->num_vertices = num_vertices;
    object_ptr->num_faces = num_faces;
    
    return (0);
}

//FUNCIÓN AUXILIAR QUE CÁLCULA EL PUNTO CENTRAL DE UNA CARA
void calculate_face_center(face *const f, const vertex *const vertex_table){
    unsigned int i;
    punto3 center;
    const vertex *v;
    
    center = (punto3){0, 0, 0};
    for(i = 0; i < f->num_vertices; ++i){
        v = vertex_table + (f->vertex_ind_table[i]);
        center.x += v->coord.x;
        center.y += v->coord.y;
        center.z += v->coord.z;
    }
    center.x /= f->num_vertices;
    center.y /= f->num_vertices;
    center.z /= f->num_vertices;
    f->center = center;
}

//FUNCIÓN AUXILIAR QUE CÁLCULA EL PUNTO = CENTRO + NORMAL_SC*NORMAL DE UNA CARA
void calculate_face_normal(face *const f, const vertex *const vertex_table){
    punto3 c;
    vector n;   //normal de la cara en el sistema local del objeto
    punto3 p1, p2, p3;

    p1 = (vertex_table + (f->vertex_ind_table[0]))->coord;
    p2 = (vertex_table + (f->vertex_ind_table[1]))->coord;
    p3 = (vertex_table + (f->vertex_ind_table[2]))->coord;
    n = calculate_normal_vector(p1, p2, p3);
    c = f->center;
    f->N = (punto3){c.x + NORMAL_SC*n.x, c.y + NORMAL_SC*n.y, c.z + NORMAL_SC*n.z};
}

//FUNCIÓN AUXILIAR QUE CÁLCULA EL PUNTO = COORD + NORMAL_SC*NORMAL DE UN VÉRTICE
void calculate_vertex_normal(
    vertex *const v, const unsigned int v_ind, 
    const face *const face_table, const unsigned int num_faces)
{
    unsigned int i, j;
    const face *f;
    vector n;

    n = (vector){0, 0, 0};
    for(i = 0; i < num_faces; ++i){
        f = face_table + i;
        for(j = 0; j < f->num_vertices; ++j){
            if(v_ind == f->vertex_ind_table[j]){
                n = calculate_sum_vectors(n, calculate_vector_AtoB3(f->center, f->N)); //no hace falta normalizarlos, todos tienen longitud 40
                break;
            }
        }
    }

    n = normalized(n);
    v->N = (punto3){v->coord.x + NORMAL_SC*n.x, v->coord.y + NORMAL_SC*n.y, v->coord.z + NORMAL_SC*n.z};
}

//
//FUNCIÓN QUE CARGA EL OBJETO DESDE SU FICHERO Y LO INTRODUCE EN LA LISTA DE OBJETOS
//
void load_from_file(
    const char *const file_name, object **const firstptrptr, object **const selptrptr,
    const color3 ka, const color3 kd, const color3 ks, const real_t ns)
{
    unsigned int i;
    object *optr;
    punto3 center;
    vertex *v;

    optr = (object*) malloc(sizeof(object));
    //color por defecto en modo línea: blanco
    optr->rgb = (color3){255, 255, 255};
    read_wavefront(file_name, optr);

    //matrices de transformación del objeto
    optr->mptr = (mlist *)malloc(sizeof(mlist));
    for (i=0; i<16; i++) optr->mptr->m[i] = 0;
    optr->mptr->m[0] = 1.0;
    optr->mptr->m[5] = 1.0;
    optr->mptr->m[10] = 1.0;
    optr->mptr->m[15] = 1.0;
    optr->mptr->hptr = 0;

    //introducción al inicio de la lista de objetos + pasa a ser el seleccionado
    optr->hptr = *firstptrptr;
    *firstptrptr = optr;
    *selptrptr = optr;

    //parámetros que definen el color del objeto; es decir, su interacción con las luces
    optr->ka = ka;
    optr->kd = kd;
    optr->ks = ks;
    optr->ns = ns;

    //número de transformaciones iniciales en modo análisis a 0
    optr->num_transf_analisis_mode = 0;

    //el punto central del objeto == media de cada componente x,y,z de todos los vértices
    center = (punto3){0, 0, 0};
    for(i = 0; i < optr->num_vertices; ++i){
        v = optr->vertex_table + i;
        center.x += v->coord.x;
        center.y += v->coord.y;
        center.z += v->coord.z;
    }
    center.x /= optr->num_vertices;
    center.y /= optr->num_vertices;
    center.z /= optr->num_vertices;
    optr->center = center;

    //cálculo del punto central y del vector normal de las caras
    for(i = 0; i < optr->num_faces; ++i){
        calculate_face_center(optr->face_table + i, optr->vertex_table);
        calculate_face_normal(optr->face_table + i, optr->vertex_table);
    }

    //cálculo del vector normal de los vértices
    for(i = 0; i < optr->num_vertices; ++i){
        calculate_vertex_normal(optr->vertex_table + i, i, optr->face_table, optr->num_faces);
    }

    printf("Objeto %s cargado!\n", file_name);
}