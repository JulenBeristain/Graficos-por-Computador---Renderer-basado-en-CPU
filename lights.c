#include <math.h>
#include <stdlib.h>
#include "lights.h"
#include "transformations.h"
#include "matrix_operations.h"

#define ENVIRONMENT_COLOR       WHITE
#define SUN_COLOR               WHITE
#define SUN_DIR                 ((vector){0, -1, 0})        //12 en punto
#define BULB_COLOR              WHITE
#define BULB_POS                ((punto3){0, 0, -200})
#define SEL_OBJ_COLOR           WHITE
#define SEL_CAM_COLOR           WHITE
#define SEL_OBJ_APERTURE_COS    (cos(PI / 8)) //22.5º, en total, *2 = 45º
#define SEL_CAM_APERTURE_COS    SEL_OBJ_APERTURE_COS

//parámetro que define la concentración de las luces de tipo SPOT (focos)
//si 0, ilumina uniformemente todo el volumen de iluminación (cono)
#define FOCO_UNIFORME       0
#define FOCO_CONCENTRADO    5
#define P_FOCO              FOCO_CONCENTRADO

//Definición de la atenuación por distancia para luces posicionales y focos
static inline real_t distance_attenuation(real_t d){
    return 1 / (1 + 0.000005*d + 0.000005*d*d);
}

real_t delta_aperture_sin;
real_t delta_aperture_cos;
real_t max_aperture_cos;
real_t min_aperture_cos;

light lights[NUM_LIGHTS];

void init_lights(void){
    unsigned int i;

    //inicializar constantes que determinan el aumento y disminución del ángulo de apertura de las
    //luces tipo SPOT (focos del objeto/camara seleccinados)
    delta_aperture_sin = sin(PI / 36); //5º
    delta_aperture_cos = cos(PI / 36); //5º
    max_aperture_cos   = 0;            //90º
    min_aperture_cos   = 1;            //0º
        
    //luz ambiental
    lights[ENVIRONMENT] = (light){
        .onoff = 1,
        .type = ENVIRONMENT,
        .I = ENVIRONMENT_COLOR,
        .mptr = 0
    };
    
    //luz 0: sol
    lights[SUN] = (light){
        .onoff = 1,
        .type = DIRECTIONAL,
        .I = SUN_COLOR,
        .dir = SUN_DIR,
        .mptr = (mlist*) malloc(sizeof(mlist))
    };
    lights[SUN].mptr->hptr = 0;
    for(i = 0; i < 16; ++i) lights[SUN].mptr->m[i] = 0;
    for(i = 0; i < 4; ++i) lights[SUN].mptr->m[4*i + i] = 1;

    //luz 1: bombilla
    lights[BULB] = (light){
        .onoff = 1,
        .type = POSITIONAL,
        .I = BULB_COLOR,
        .pos = BULB_POS,
        .mptr = (mlist*) malloc(sizeof(mlist))
    };
    lights[BULB].mptr->hptr = 0;
    for(i = 0; i < 16; ++i) lights[BULB].mptr->m[i] = 0;
    for(i = 0; i < 4; ++i) lights[BULB].mptr->m[4*i + i] = 1;

    //luz 2: objeto seleccionado
    lights[SEL_OBJ] = (light){
        .onoff = 1,
        .type = SPOT,
        .I = SEL_OBJ_COLOR,
        //.pos = (punto3){-100, 0, 0},
        //.dir = (vector){0, 0, -1},
        .aperture_cos = SEL_OBJ_APERTURE_COS,
        .mptr = 0
    };

    //luz 3: camara seleccionada
    lights[SEL_CAM] = (light){
        .onoff = 1,
        .type = SPOT,
        .I = SEL_CAM_COLOR,
        //.pos = (punto3){0, 0, 400},
        //.dir = (vector){0, 0, -1},
        .aperture_cos = SEL_CAM_APERTURE_COS,
        .mptr = 0
    };
}

//
// FUNCION AUXILIAR QUE ACUMULA INTESIDADES EL PRODUCTO DE DOS VECTORES COLOR3
//

//GENERAL
void accumulate_intensity(color3 *Iptr, const real_t coeff, const color3 light_intensity, const color3 k){
    Iptr->r += coeff * light_intensity.r * k.r;
    Iptr->g += coeff * light_intensity.g * k.g;
    Iptr->b += coeff * light_intensity.b * k.b;
}

//DIFUSIÓN
void accumulate_difused_intensity(
    color3 *Iptr, const vector N, const vector L, const color3 light_intensity, const color3 kd,
    const real_t distance_attenuation)
{
    const real_t cos_sp = fmax(0, scalar_product(N, L));
    accumulate_intensity(Iptr, cos_sp * distance_attenuation, light_intensity, kd);
}

//ESPECULAR
void accumulate_specular_reflect(
    color3 *Iptr, const vector N, const vector V, const vector L, 
    const color3 light_intensity, const color3 ks, const real_t ns,
    const real_t distance_attenuation)
{
    const vector H = normalized(calculate_sum_vectors(V, L));
    const real_t cos_sp = pow(fmax(0, scalar_product(N, H)), ns);
    accumulate_intensity(Iptr, cos_sp * distance_attenuation, light_intensity, ks);
}
void accumulate_specular_reflect_exact(
    color3 *Iptr, const vector N, const vector V, const vector L,
    const color3 ligth_intensity, const color3 ks, const real_t ns,
    const real_t distance_attenuation)
{
    const vector R = calculate_subtraction_vectors( scalar_multiplication( N, 2 * scalar_product(N, L) ), L );
    const real_t cos_sp = pow(fmax(0, scalar_product(V, R)), ns);
    accumulate_intensity(Iptr, cos_sp * distance_attenuation, ligth_intensity, ks);
}
#define accumulate_specular_intensity accumulate_specular_reflect_exact

//PARA LUCES TIPO FOCO (SPOT)
void accumulate_spot_intensity(
    color3 *Iptr, const vector N, const vector V, const vector L,
    const vector F, const real_t aperture_cos, const color3 light_intensity, 
    const color3 kd, const color3 ks, const real_t ns, real_t distance_attenuation)
{
    real_t foco_coef;
    color3 spot_intensity;

    const real_t cos_sp = scalar_product(F, opposite(L));
    if(aperture_cos < cos_sp){     //el punto está en el volumen (cono) de iluminación (y sabemos que cos_sp no es negativo)
        //intensidad difusa de las fuentes de luz
        foco_coef = pow(cos_sp, P_FOCO);
        spot_intensity = (color3){light_intensity.r * foco_coef, 
                                  light_intensity.g * foco_coef, 
                                  light_intensity.b * foco_coef};
        
        accumulate_difused_intensity(Iptr, N, L, spot_intensity, kd, distance_attenuation);
        accumulate_specular_intensity(Iptr, N, V, L, spot_intensity, ks, ns, distance_attenuation);
    }
}

//
// FUNCIÓN PARA CALCULAR LA INTENSIDAD DE ILUMINACIÓN EN UN PUNTO
// Precondiciones:  p y N están en el SRObservador
//                  lights[...].campos/camdir están en el SRObservador
color3 calculate_intensity(const punto3 p, const object *const material_info_ptr, const vector N){
    color3 I = {0, 0, 0};
    vector F;
    real_t dist_atten, dist;

    const color3 ka = material_info_ptr->ka;
    const color3 kd = material_info_ptr->kd;
    const color3 ks = material_info_ptr->ks;
    const real_t ns = material_info_ptr->ns;

    //Vector del punto al observador: en el SRObservador, V == p->O
    const vector V = normalized(opposite((vector){p.x, p.y, p.z}));
    const vector Lsun = opposite(lights[SUN].camdir);
    const vector Lbulb = normalized(calculate_vector_AtoB3(p, lights[BULB].campos));
    const vector Lselobj = normalized(calculate_vector_AtoB3(p, lights[SEL_OBJ].campos));
    const vector Lselcam = normalized(calculate_vector_AtoB3(p, lights[SEL_CAM].campos));

    //luz ambiental
    if(lights[ENVIRONMENT].onoff){
        accumulate_intensity(&I, 1, lights[ENVIRONMENT].I, ka);
    }

    //sol
    if(lights[SUN].onoff){
        accumulate_difused_intensity(&I, N, Lsun, lights[SUN].I, kd, 1);
        accumulate_specular_intensity(&I, N, V, Lsun, lights[SUN].I, ks, ns, 1);
    }
    
    //bombilla
    if(lights[BULB].onoff){
        dist = calculate_vector_module(calculate_vector_AtoB3(p, lights[BULB].campos));
        dist_atten = distance_attenuation(dist); //fmin(1, distance_attenuation(dist));
        accumulate_difused_intensity(&I, N, Lbulb, lights[BULB].I, kd, dist_atten);
        accumulate_specular_intensity(&I, N, V, Lsun, lights[BULB].I, ks, ns, dist_atten);
    }
    
    //objeto seleccionado
    if(lights[SEL_OBJ].onoff){
        F = lights[SEL_OBJ].camdir;
        dist = calculate_vector_module(calculate_vector_AtoB3(p, lights[SEL_OBJ].campos));
        dist_atten = distance_attenuation(dist); //fmin(1, distance_attenuation(dist));
        accumulate_spot_intensity(
            &I, N, V, Lselobj,
            F, lights[SEL_OBJ].aperture_cos, lights[SEL_OBJ].I, 
            kd, ks, ns, dist_atten);
    }
    

    //camara seleccionada
    if(lights[SEL_CAM].onoff){
        F = lights[SEL_CAM].camdir;
        dist = calculate_vector_module(calculate_vector_AtoB3(p, lights[SEL_CAM].campos));
        dist_atten = distance_attenuation(dist); //fmin(1, distance_attenuation(dist));
        accumulate_spot_intensity(
            &I, N, V, Lselcam,
            F, lights[SEL_CAM].aperture_cos, lights[SEL_CAM].I, 
            kd, ks, ns, dist_atten);
    }
    
    return I;
}
