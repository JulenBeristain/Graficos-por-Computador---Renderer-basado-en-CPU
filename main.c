//	Program developed by Julen Beristain
//	
//	Informatika Fakultatea
//	Euskal Herriko Unibertsitatea
//	http://www.ehu.eus/if
//
// to compile it: gcc *.c -lGL -lGLU -lglut -lm             (Linux)
//                gcc *.c -lGlU32 -lOpenGL32 -lfreeglut -lm (Windows)
//

#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "transformations.h"
#include "matrix_operations.h"
#include "dibujar_poligono.h"
#include "dibujar_triangulo.h"
#include "camaras.h"
#include "obj.h"
#include "load_obj.h"
#include "lights.h"
#include "seleccionar_material.h"

// DIMENSIONES Y POSICIONAMIENTO DE LA VENTANA
enum {
    WINDOW_WIDTH = 500,
    WINDOW_HEIGHT = 500,
    WINDOW_X = 500,
    WINDOW_Y = 100
};

//
// VARIABLES GLOBALES
//

// OBJETOS
unsigned int indexx; //indice del face a dibujar
object *foptr;
object *sel_ptr;

// OPCIONES
bool denak;
bool lineak;
bool objektuak;
char aldaketa;
bool ald_lokala;
bool mostrar_normales_caras;
bool mostrar_normales_vertices;
bool mostrar_caras_traseras;

// CÁMARAS
// Opciones de Cámara
unsigned char transformations_to;
bool observer;
bool proyection;
bool camara_mode;

// Datos de Cámara
camara *fcptr;
camara *sel_cam_ptr;

//ILUMINACIÓN
bool take_normals_of;
unsigned int sel_luz;


// BUFFER PARA NOMBRES DE FICHEROS
char fitxiz[100];


//
// FUNCIONES AUXILIARES PARA MARRAZTU
//
// DIBUJAR TODOS LOS POLIGONOS DE UN OBJETO
void dibujar_objeto(
    object *optr, real_t *const mcsr_transf, const real_t *const mcsr,
    funcDibujarPoligono f_dibujar_poligono) 
{
    unsigned int i;

    mxm(mcsr_transf, mcsr, optr->mptr->m);

    for(i = 0; i < optr->num_faces; ++i){
        //mcsr se sigue necesitando para los vectores normales
        f_dibujar_poligono(optr, optr->face_table + i, mcsr_transf, mcsr);
    }
}

//
// FUNCIÓN PARA DIBUJAR LA BOMBILLA (para poder visualizar mejor su posición)
//
void drawBulb(const punto3 center) {
    const real_t cx = center.x, cy = center.y, cz = center.z;
    const real_t hd = 1.0f; //half diagonal (diagonales 2 y 2)
    const color3 color = YELLOW;

    //DIBUJAR UN DIAMANTE (dos piramides pegadas por sus bases)
    //Definición de sus seis puntos
    const punto3 v_sup = (punto3){cx, cy + hd, cz};
    const punto3 v_med_der_front = (punto3){cx + hd, cy, cz + hd};
    const punto3 v_med_izq_front = (punto3){cx - hd, cy, cz + hd};
    const punto3 v_med_der_back  = (punto3){cx + hd, cy, cz - hd};
    const punto3 v_med_izq_back  = (punto3){cx - hd, cy, cz - hd};
    const punto3 v_inf = (punto3){cx, cy - hd, cz};
    //Triangulos superiores
    //Frontal
    dibujar_triangulo_color(v_sup, v_med_der_front, v_med_izq_front, color);
    //Derecha
    dibujar_triangulo_color(v_sup, v_med_der_front, v_med_der_back, color);
    //Izquierda
    dibujar_triangulo_color(v_sup, v_med_izq_front, v_med_izq_back, color);
    //Trasera
    dibujar_triangulo_color(v_sup, v_med_der_back, v_med_izq_back, color);
    //Triangulos inferiores
    //Frontal
    dibujar_triangulo_color(v_inf, v_med_der_front, v_med_izq_front, color);
    //Derecha
    dibujar_triangulo_color(v_inf, v_med_der_front, v_med_der_back, color);
    //Izquierda
    dibujar_triangulo_color(v_inf, v_med_izq_front, v_med_izq_back, color);
    //Trasera
    dibujar_triangulo_color(v_inf, v_med_der_back, v_med_izq_back, color);
}

// FUNCIÓN DE DIBUJADO PARA OPENGL
static void marraztu(void)
{
    object *auxptr;
    real_t mcsr[16];
    real_t mcsr_transf[16]; //para multiplicar mcsr y mtransformacion del objeto una única vez
    vector v_sol_mundo;
    punto3 origen, p_dir_sol, p_pos_bombilla, p_dir_sel_obj, p_dir_sel_cam;
    funcDibujarPoligono f_dibujar_poligono;
    
    // marrazteko objektuak behar dira
    // no se puede dibujar sin objetos
    if (foptr == 0) return;

    // clear viewport...
    if (objektuak == 1) glClear( GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT );
    else if (denak == 0) glClear( GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT );
    
    //ACTUALIZACION MATRIZ OPENGL DE PROYECCION
    assert(proyection == PARALELO || proyection == PERSPECTIVA);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (proyection == PARALELO){
        glOrtho(OLEFT, ORIGHT, OBOTTOM, OTOP, ONEAR, OFAR);
    } else { // if (proyection == PERSPECTIVA) {
        glFrustum(LEFT, RIGHT, BOTTOM, TOP, NEAR_, FAR_);
    }

    //CALCULO DEL SISTEMA DE REFERENCIA
    assert(observer == CAMARA || observer == OBJETO);
    if(observer == CAMARA){
        //Calcular la matriz de cambio de sistema de referencia a partir de mobj de la cámara seleccionada
        mobj_to_mcsr(mcsr, sel_cam_ptr->mptr->m);
    } else { //if (observer == OBJETO) {
        //Desde el punto de vista del objeto
        mobj_to_mcsr(mcsr, sel_ptr->mptr->m);
    }
    
    //Pasar al SRObservador todo lo que no dependa de los datos de los objetos a dibujar
    //V: la posición del observador en su SR es el origen 0
    //L_sol:
    v_sol_mundo = mxv(lights[SUN].mptr->m, lights[SUN].dir);
    mxp3(&origen, mcsr, (punto3){0, 0, 0});
    mxp3(&p_dir_sol, mcsr, (punto3){v_sol_mundo.x, v_sol_mundo.y, v_sol_mundo.z});
    lights[SUN].camdir = normalized(calculate_vector_AtoB3(origen, p_dir_sol));
    //L_bombilla
    mxp3(&p_pos_bombilla, lights[BULB].mptr->m, lights[BULB].pos);
    mxp3(&lights[BULB].campos, mcsr, p_pos_bombilla);
    //L_sel_obj: pos y dir ya en el SRMundo
    mxp3(&lights[SEL_OBJ].campos, mcsr, lights[SEL_OBJ].pos);
    mxp3(&p_dir_sel_obj, mcsr, (punto3){lights[SEL_OBJ].dir.x, lights[SEL_OBJ].dir.y, lights[SEL_OBJ].dir.z});
    lights[SEL_OBJ].camdir = normalized(calculate_vector_AtoB3(origen, p_dir_sel_obj));
    //L_sel_cam: pos y dir ya en el SRMundo
    mxp3(&lights[SEL_CAM].campos, mcsr, lights[SEL_CAM].pos);
    mxp3(&p_dir_sel_cam, mcsr, (punto3){lights[SEL_CAM].dir.x, lights[SEL_CAM].dir.y, lights[SEL_CAM].dir.z});
    lights[SEL_CAM].camdir = normalized(calculate_vector_AtoB3(origen, p_dir_sel_cam));


    //Dependiendo del estado del programa, configurar las funciones de dibujado   
    f_mostrar_normal_cara = mostrar_normales_caras ? mostrar_normal_cara : no_mostrar_normal_cara;
    f_mostrar_normal_vertice = mostrar_normales_vertices ? mostrar_normal_vertice : no_mostrar_normal_vertice;
    f_is_looking_backwards = (proyection == PARALELO) ? is_looking_backwards_paralelo : is_looking_backwards_perspectiva;
    f_dibujar_poligono = tabla_dibujar_poligono[indice_dibujar_poligono()];

    //DIBUJAR OBJETOS
    if (objektuak == 1){
        if (denak == 1){
            for (auxptr = foptr; auxptr != 0; auxptr = auxptr->hptr){
                dibujar_objeto(auxptr, mcsr_transf, mcsr, f_dibujar_poligono);
            }
        }
        else{
            dibujar_objeto(sel_ptr, mcsr_transf, mcsr, f_dibujar_poligono);
        }
    }
    else{
        mxm(mcsr_transf, mcsr, sel_ptr->mptr->m);
        f_dibujar_poligono(sel_ptr, sel_ptr->face_table + indexx, mcsr_transf, mcsr);
    }
    
    //DIBUJAR CAMARAS
    // dibujar también todas las cámaras, independientemente de la opción objektuak o denak
    for (auxptr = fcptr; auxptr != 0; auxptr = auxptr->hptr){
        dibujar_objeto(auxptr, mcsr_transf, mcsr, f_dibujar_poligono);
    }

    //DIBUJAR UN DIAMANTE EN LA POSICIÓN DE LA BOMBILLA (si está encendida)
    if(lights[BULB].onoff){
        drawBulb(lights[BULB].campos);
    }
    

    glFlush();
}

//
// FUNCIÓN DE TECLADO PARA OPENGL
//

//FUNCIÓN AUXILIAR PARA VISUALIZAR TODAS LAS OPCIONES GLOBALES TRAS PULSAR UNA TECLA
void print_global_options(void){
    unsigned int i;
    object *auxptr;

    printf("\n\tOPCIONES GLOBALES:\n");
    printf("DENAK: %u\n", denak);
    printf("LINEAK: %u\n", lineak);
    printf("OBJEKTUAK: %u\n", objektuak);
    printf("ALDAKETA: %c\n", aldaketa);
    printf("ALDAKETA LOKALA: %u\n", ald_lokala);
    printf("MOSTRAR NORMALES CARAS: %u\n", mostrar_normales_caras);
    printf("MOSTRAR NORMALES VERTICES: %u\n", mostrar_normales_vertices);
    printf("MOSTRAR CARAS TRASERAS: %u\n", mostrar_caras_traseras);
    printf("TRANSFORMATIONS TO (camara 0 - objeto 1 - luz 2): %u\n", transformations_to);
    printf("OBSERVER: %u\n", observer);
    printf("PROYECTION (paralelo 0 - perspectiva 1): %u\n", proyection);
    printf("CAMARA MODE (analisis 0 - vuelo 1): %u\n", camara_mode);
    
    for(i = 0, auxptr = foptr; auxptr != sel_ptr; ++i, auxptr = auxptr->hptr);
    printf("SEL_OBJ (starting from 0): %d\n", i);
    for(i = 0, auxptr = fcptr; auxptr != sel_cam_ptr; ++i, auxptr = auxptr->hptr);
    printf("SEL_CAM (starting from 0): %d\n", i);
    printf("SEL_LUZ (0: Ia - 1: Sol - 2: Bombilla - 3: Sel_obj - 4: Sel_cam): %u\n", sel_luz);
    
    printf("\n");
}

// FUNCIÓN AUXILIAR PARA INVERTIR BOOLEANOS
static inline bool invert(bool option){
    return ~option & 1;
}

//FUNCIÓN AUXILIAR PARA ACTUALIZAR LA POSICIÓN Y DIRECCIÓN DE LAS LUCES (SOLO FOCOS)
static void update_sel_ligth(const object *const selptr, const int sel){
    const real_t *m = selptr->mptr->m;
    
    lights[sel].pos = mobj_to_e(m);
    lights[sel].dir = opposite(mobj_to_z(m));
}

// FUNCION AUXILIAR PARA QUE LA CAMARA SELECCIONADA MIRE HACIA EL OBJETO SELECCIONADO
void lookAtSelectedObj(){
    punto3 target;

    mxp3(&target, sel_ptr->mptr->m, sel_ptr->center);
    lookAt(sel_cam_ptr, target);
    printf("NUM PRE: %u\n", sel_cam_ptr->num_transf_analisis_mode);
    ++(sel_cam_ptr->num_transf_analisis_mode);
    printf("NUM POST: %u\n", sel_cam_ptr->num_transf_analisis_mode);

    //cambiar la direccion de la luz de la cámara seleccionada
    lights[SEL_CAM].dir = opposite(mobj_to_z(sel_cam_ptr->mptr->m));
}

// FUNCION AUXILIAR PARA LAS TRANSFORMACIONES A CAMARAS Y OBJETOS (TECLADOS X,Y,Z)
void aldatu(
    funcAldaketaCamaraVuelo     aldaketa_cam_vuelo,
    funcAldaketaCamaraAnalisis  aldaketa_cam_analisis,
    funcAldaketaObjeto          aldaketa_objeto,
    funcAldaketaLuzDireccional  aldaketa_luz_direccional,
    funcAldaketaLuzPosicional   aldaketa_luz_posicional,
    int dir)
{
    punto3 target;

    assert(transformations_to == CAMARA || transformations_to == OBJETO || transformations_to == LUZ);
    if(transformations_to == CAMARA){
        assert(camara_mode == VUELO || camara_mode == ANALISIS);
        if(camara_mode == VUELO){
            aldaketa_cam_vuelo(sel_cam_ptr, dir);
        } else { //ANALISIS
            mxp3(&target, sel_ptr->mptr->m, sel_ptr->center);
            aldaketa_cam_analisis(sel_cam_ptr, target, dir);
            ++(sel_cam_ptr->num_transf_analisis_mode);
        }
        //actualizar la posicion y la direccion de la luz de la cámara seleccionada
        update_sel_ligth(sel_cam_ptr, SEL_CAM);

    } else if (transformations_to == OBJETO) {
        aldaketa_objeto(sel_ptr, dir);

        //En modo analisis, volver a apuntar al objeto despues de cada transformacion
        if(camara_mode == ANALISIS){
            lookAtSelectedObj();
        }

        //actualizar la posicion y la direccion de la luz del objeto seleccionado
        update_sel_ligth(sel_ptr, SEL_OBJ);
    
    } else { // transformations_to == LUZ
        if(lights[sel_luz].type == DIRECTIONAL){
            aldaketa_luz_direccional(&(lights[sel_luz]), dir);
        } else if (lights[sel_luz].type == POSITIONAL){
            aldaketa_luz_posicional(&(lights[sel_luz]), dir);
        }
    }
}

void reinitializeAllNumTransfAnalisisMode(){
    camara *camptr;
    for(camptr = fcptr; camptr != 0; camptr = camptr->hptr){
        printf("NUM PRE: %u", camptr->num_transf_analisis_mode);
        camptr->num_transf_analisis_mode = 0;
        printf("NUM POST: %u", camptr->num_transf_analisis_mode);
    }
}

// FUNCIÓN AUXILIAR PARA QUE CUANDO VOLVAMOS AL MODO VUELO LOS CONTEOS DE TODAS LAS CÁMARAS
// DE SU NÚMERO DE TRANSFORMACIONES EN MODO ANÁLISIS VUELVA A CERO
void vueltaAlModoVuelo(){
    camara_mode = VUELO;
    reinitializeAllNumTransfAnalisisMode();
}

// This function will be called whenever the user pushes one key
static void teklatua (unsigned char key, int x, int y)
{
    real_t old_aperture_cos, new_aperture_cos;
    color3 ka, kd, ks;
    real_t ns;

    switch(key)
	{
        //MODO SOLO DIBUJAR TRIANGULOS
	    case 13: //ENTER
            assert(foptr != 0); // No damos la opción de eliminar objetos, por lo que es imposible
            indexx ++;          // pero si es el último? hay que controlarlo!
            if (indexx == sel_ptr->num_faces){
                indexx = 0;
                if ((denak == 1) && (objektuak == 0)){
                    glClear( GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT );
                    glFlush();
                }
            }
		    
		    break;
        
        //MODO DE DIBUJAR: TODOS LOS TRIANGULOS/OBJETOS, TRIANGULOS/OBJETOS, LINEAS/AREAS
        case 'd':
        case 'D':
            denak = invert(denak);
            break;
        case 'o':
        case 'O':
            objektuak = invert(objektuak);
            break;
        case 'l':
        case 'L':
            lineak = invert(lineak);
            break;

        //SELECCIÓN DE LA TRANSFORMACION A APLICAR
        case 't':
        case 'T':
            aldaketa = 't';
            break;
        case 'r':
        case 'R':
            aldaketa = 'r';
            break;
        case 's':
        case 'S':
            //En caso de estar transformando a las cámaras, como no permito escalados a cámaras
            //tampoco permito cambiar de modo a transformación escala.
            //En caso de estar en aldaketa 's' en transformaciones a objetos y pasar a transformaciones
            //a camaras, no modifico aldaketa. El usuario tendrá que elegir 'r' o 't'.
            //Lo mismo para las luces.
            if(transformations_to == OBJETO) aldaketa = 's';
            break;

        //TRANSFOMATIONS_TO CAMARAS: MODO_CAMARA VUELO/ANALISIS
        //                  OBJETOS: ALDAKETA LOKALA/GLOBALA
        case 'g':
        case 'G':
            assert(transformations_to == OBJETO || transformations_to == CAMARA || transformations_to == LUZ);
            if(transformations_to == OBJETO){
                ald_lokala = invert(ald_lokala);
            } else if (transformations_to == CAMARA){
                camara_mode = invert(camara_mode);
                if(camara_mode == ANALISIS){
                    lookAtSelectedObj();
                } else if(camara_mode == VUELO){
                    vueltaAlModoVuelo();
                }
            } //else, con Luces, no hacemos nada
            break;
        
        //APLICAR TRANSFORMACIÓN
        case 'x':
            aldatu(x_aldaketa_cam_vuelo, x_aldaketa_cam_analisis, x_aldaketa,
                   x_aldaketa_luz_direccional, x_aldaketa_luz_posicional, 1);
            break;
        case 'y':
            aldatu(y_aldaketa_cam_vuelo, y_aldaketa_cam_analisis, y_aldaketa,
                   y_aldaketa_luz_direccional, y_aldaketa_luz_posicional, 1);
            break;
        case 'z':
            aldatu(z_aldaketa_cam_vuelo, z_aldaketa_cam_analisis, z_aldaketa,
                   z_aldaketa_luz_direccional, z_aldaketa_luz_posicional, 1);
            break;
        case 'X':
            aldatu(x_aldaketa_cam_vuelo, x_aldaketa_cam_analisis, x_aldaketa,
                   x_aldaketa_luz_direccional, x_aldaketa_luz_posicional, 0);
            break;
        case 'Y':
            aldatu(y_aldaketa_cam_vuelo, y_aldaketa_cam_analisis, y_aldaketa,
                   y_aldaketa_luz_direccional, y_aldaketa_luz_posicional, 0);
            break;
        case 'Z':
            aldatu(z_aldaketa_cam_vuelo, z_aldaketa_cam_analisis, z_aldaketa,
                   z_aldaketa_luz_direccional, z_aldaketa_luz_posicional, 0);
            break;
        
        //DESHACER TRANSFORMACIÓN
        case 'u':
        case 'U':
            assert(transformations_to == CAMARA || transformations_to == OBJETO || transformations_to == LUZ);
            if(transformations_to == CAMARA){
                undo(sel_cam_ptr);

                printf("\nPRE - Num_transf_analisis_mode: %d\n", sel_cam_ptr->num_transf_analisis_mode);
                if(camara_mode == ANALISIS && --(sel_cam_ptr->num_transf_analisis_mode) == 0){
                    printf("\nPOST - Num_transf_analisis_mode: %d\n", sel_cam_ptr->num_transf_analisis_mode);
                    vueltaAlModoVuelo();
                }
                //actualizar la posicion y direccion de la luz de la camara seleccionada
                update_sel_ligth(sel_cam_ptr, SEL_CAM);

            } else if (transformations_to == OBJETO) {
                undo(sel_ptr);

                if(camara_mode == ANALISIS){
                    lookAtSelectedObj();
                }

                //actualizar la posicion y direccion de la luz del objeto seleccionado
                update_sel_ligth(sel_ptr, SEL_OBJ);
            
            } else { // transformations_to == LUZ
                if(lights[sel_luz].type == DIRECTIONAL || lights[sel_luz].type == POSITIONAL){
                    undo_list(&(lights[sel_luz].mptr));
                }
            }
            break;

        //Elección del tipo de "ente" a transformar
        case 'c':
            //0 - CAMARA (inicialización)
            //1 - OBJETO
            //2 - LUZ
            transformations_to = (transformations_to + 1) % 3;
            break;

        //Configuración de la camara: observador y modo de proyección
        case 'C':
            observer = invert(observer);
            break;
        case 'p':
        case 'P':
            proyection = invert(proyection);
            break;

        //Selección de la opción para mostrar caras traseras o vectores normales
        case 'b':
        case 'B':
            mostrar_caras_traseras = invert(mostrar_caras_traseras);
            break;
        case 'n':
        case 'N':
            mostrar_normales_caras = invert(mostrar_normales_caras);
            break;
        case 'm':
        case 'M':
            mostrar_normales_vertices = invert(mostrar_normales_vertices);
            break;

        //ILUMINACIÓN
        // Luz ambiental - Sol - Bombilla - Foco objeto seleccionado - Foco camara seleccionada
        case '0':
            lights[0].onoff = invert(lights[0].onoff);
            break;
        case '1':
            lights[1].onoff = invert(lights[1].onoff);
            break;
        case '2':
            lights[2].onoff = invert(lights[2].onoff);
            break;
        case '3':
            lights[3].onoff = invert(lights[3].onoff);
            break;
        case '4':
            lights[4].onoff = invert(lights[4].onoff);
            break;

        //La 'F' ya la uso para cargar cámaras nuevas. Elijo 'i'/'I' de iluminación.
        case 'i':
        case 'I':
            //Cambiar los vectores normales que considero al calcular la iluminación
            //Normales de las caras/polígonos o de los vértices
            take_normals_of = invert(take_normals_of);
            break;
	    
        //En caso de transformar luces de tipo foco, incrementar/decrementar apertura
        case '+':
            //aperture (ahora aperture_cos) lo interpretamos como el coseno del angulo y no el angulo en sí,
            //para evitar miles de evaluaciones funcionales (uno por cada vértice del objeto)        
            if(transformations_to == LUZ && lights[sel_luz].type == SPOT){
                //cosine of the sum
                old_aperture_cos = lights[sel_luz].aperture_cos;
                new_aperture_cos = old_aperture_cos*delta_aperture_cos - sin(acos(old_aperture_cos))*delta_aperture_sin;
                //angulo aumenta --> coseno disminuye
                //cuando llega a 0, está en 90º, el máximo permitido
                //a partir de entonces, al seguir incrementando el angulo en el segundo cuadrante
                //el coseno pasa a ser negativo, entonces NO hay que actualizar el coseno de la apertura
                if(new_aperture_cos >= 0){
                    lights[sel_luz].aperture_cos = new_aperture_cos;
                } else { //Nos aseguramos de que el angulo de apertura sea 90º
                    lights[sel_luz].aperture_cos = 0;
                }
            }
            break;
        case '-':
            if(transformations_to == LUZ && lights[sel_luz].type == SPOT){
                //cosine of the difference
                old_aperture_cos = lights[sel_luz].aperture_cos;
                new_aperture_cos = old_aperture_cos*delta_aperture_cos + sin(acos(old_aperture_cos))*delta_aperture_sin;
                //angulo disminuye --> coseno aumenta
                //cuando llega a 1, está en 0º, el mínimo posible
                //a partir de entonces, al seguir disminuyendo el angulo a los angulos negativos del cuarto cuadrante
                //el nuevo coseno sería menor que el viejo, entonces NO hay que actualizar el coseno de la apertura
                if(new_aperture_cos >= old_aperture_cos){
                    lights[sel_luz].aperture_cos = new_aperture_cos;
                } else { //Nos aseguramos de que el angulo de apertura sea 0º --> el foco no ilumine nada
                    lights[sel_luz].aperture_cos = 1;
                }
            }
            break;

        //Extra: modificar la intensidad de la luz seleccionada
        case 'q': //rojo - por cercanía con 'a' de azul y porque 'r' está elegido
            if(transformations_to == LUZ && lights[sel_luz].I.r <= 254){
                ++lights[sel_luz].I.r;
            }
            break;
        case 'Q':
            if(transformations_to == LUZ && lights[sel_luz].I.r >= 1){
                --lights[sel_luz].I.r;
            }
            break;
        case 'v':
            if(transformations_to == LUZ && lights[sel_luz].I.g <= 254){
                ++lights[sel_luz].I.g;
            }
            break;
        case 'V':
            if(transformations_to == LUZ && lights[sel_luz].I.g >= 1){
                --lights[sel_luz].I.g;
            }
            break;
        case 'a':
            if(transformations_to == LUZ && lights[sel_luz].I.b <= 254){
                ++lights[sel_luz].I.b;
            }
            break;
        case 'A':
            if(transformations_to == LUZ && lights[sel_luz].I.b >= 1){
                --lights[sel_luz].I.b;
            }
            break;


        //AÑADIR OBJETOS Y CÁMARAS
        case 'f':
	        /*Ask for file*/
	        printf("idatzi fitxategi izena\n");
	        scanf("%s", &(fitxiz[0]));
            getc(stdin); //consume the newline
	        
            //Elegir un material predefinido
            seleccionar_material(&ka, &kd, &ks, &ns);

            load_from_file(
                fitxiz, &foptr, &sel_ptr,
                ka, kd, ks, ns);
	        
            indexx = 0;
            //El objeto seleccionado cambia
            update_sel_ligth(sel_ptr, SEL_OBJ);
            break;
        case 'F':
            printf("Cargando nueva camara...\n");
            
            //Elegir un material predefinido
            seleccionar_material(&ka, &kd, &ks, &ns);

            load_from_file(
                "Objetos/cam.obj", &fcptr, &sel_cam_ptr,
                ka, kd, ks, ns);

            printf("Nueva camara cargada!\n");
            //La camara seleccionada cambia
            update_sel_ligth(sel_cam_ptr, SEL_CAM);
            break;
    
        //Cambiar al siguiente OBJETO, CAMARA o LUZ (según qué estemos transformando)
        case 9: /* <TAB> */
            if (transformations_to == CAMARA){
                assert(fcptr != 0); //no puede ocurrir ya que no damos la opción a eliminar objetos
                
                sel_cam_ptr = sel_cam_ptr->hptr;
                /*The selection is circular, thus if we move out of the list we go back to the first element*/
                if (sel_cam_ptr == 0) sel_cam_ptr = fcptr;

                //Una vez seleccionada la cámara, si estamos en modo analisis, apuntar al objeto seleccionado
                if(camara_mode == ANALISIS){
                    lookAtSelectedObj();
                }

                update_sel_ligth(sel_cam_ptr, SEL_CAM);
            
            } else if (transformations_to == OBJETO){
                assert(foptr != 0); //no puede ocurrir ya que no damos la opción a eliminar objetos
            
                sel_ptr = sel_ptr->hptr;
                /*The selection is circular, thus if we move out of the list we go back to the first element*/
                if (sel_ptr == 0) sel_ptr = foptr;
                indexx = 0; // the selected polygon is the first one

                //Una vez seleccionado el objeto, si estamos en modo analisis, apuntar al objeto seleccionado
                if(camara_mode == ANALISIS){
                    //Antes de apuntar al objeto seleccionado "reiniciamos" el modo analisis, para que
                    //haciendo un undo no quedemos mirando al objeto seleccionado anterior en pleno modo
                    //analisis (donde debemos estar apuntando al objeto actualmente seleccionado)
                    reinitializeAllNumTransfAnalisisMode();
                    lookAtSelectedObj();
                }

                update_sel_ligth(sel_ptr, SEL_OBJ); //la bombilla no se mueve en ningun caso
            
            } else { //transformations_to == LUZ
                sel_luz = (sel_luz + 1) % NUM_LIGHTS;
            }
            break;
        
        //Salir de la aplicación
        case 27:  // <ESC>
            //free_memory();
            exit( 0 );
            break;
        
        default:
            printf("%d %c\n", key, key );
	}

    // The screen must be drawn to show the changes
    glutPostRedisplay();

    printf("\nTecla pulsada: %c\n", key);
    print_global_options();
}

//
// INICIALIZACIÓN DE OBJETOS Y CÁMARAS
//
static void inicializar_objetos(void){
    load_from_file(
        "Objetos/cam.obj", &foptr, &sel_ptr,
        KA_CAM, KD_CAM, KS_CAM, NS_CAM);
    traslacion_z_global(foptr, -400);

    load_from_file(
        "Objetos/x_wing.obj", &foptr, &sel_ptr,
        KA_X_WING, KD_X_WING, KS_X_WING, NS_X_WING);
    traslacion_x_global(foptr, 100);

    load_from_file(
        "Objetos/r_falke.obj", &foptr, &sel_ptr,
        KA_R_FALKE, KD_R_FALKE, KS_R_FALKE, NS_R_FALKE);
    traslacion_x_global(foptr, -100);
}
static void inicializar_camaras(void){
    load_from_file(
        "Objetos/cam.obj", &fcptr, &sel_cam_ptr,
        KA_CAM, KD_CAM, KS_CAM, NS_CAM);
    traslacion_z_global(fcptr, 400);
}
static void inicializar_luces(void){
    init_lights();
    
    //luz 2: objeto seleccionado
    update_sel_ligth(sel_ptr, SEL_OBJ);

    //luz 3: camara seleccionada
    update_sel_ligth(sel_cam_ptr, SEL_CAM);
}


//
// FUNCIÓN MAIN: PUNTO DE ENTRADA DEL PROGRAMA
//
int main(int argc, char** argv)
{
	printf("Press <ESC> to finish\n");
	glutInit(&argc,argv);
	glutInitDisplayMode ( GLUT_RGB|GLUT_DEPTH );
	glutInitWindowSize ( WINDOW_WIDTH, WINDOW_HEIGHT );
	glutInitWindowPosition ( WINDOW_X, WINDOW_Y );
	glutCreateWindow( "KBG/GC praktika" );
	
	glutDisplayFunc( marraztu );
	glutKeyboardFunc( teklatua ); 
	
    glClearColor( 0.0f, 0.0f, 0.7f, 1.0f );
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST); // activar el test de profundidad (Z-buffer)
    
    denak = 1;
    lineak = 0;
    objektuak = 1;
    aldaketa = 't';
    ald_lokala = 1;

    observer = CAMARA;
    proyection = PERSPECTIVA;
    transformations_to = CAMARA;
    camara_mode = VUELO;
    mostrar_normales_caras = 0;
    mostrar_normales_vertices = 0;
    mostrar_caras_traseras = 0;

    take_normals_of = VERTICES;

    foptr = 0;
    sel_ptr = 0;
    
    fcptr = 0;
    sel_cam_ptr = 0;

    sel_luz = SUN;

    inicializar_objetos();
    inicializar_camaras();
    inicializar_luces();

	glutMainLoop();

	return 0;
}
