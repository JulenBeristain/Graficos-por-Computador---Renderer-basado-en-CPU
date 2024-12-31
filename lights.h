#ifndef LIGHTS_H
#define LIGHTS_H

#include "obj.h"

///////////// COLOR DEFINITIONS //////////////////////////////////////
#define WHITE   ((color3){255, 255, 255})
#define PURPLE  ((color3){255, 0, 255})
#define YELLOW  ((color3){255, 255, 0})
#define ORANGE  ((color3){255, 123, 0})
#define RED     ((color3){255, 0, 0})
//////////////////////////////////////////////////////////////////////

///////////// MATERIAL DEFINITIONS ///////////////////////////////////
// See: https://people.eecs.ku.edu/~jrmiller/Courses/672/InClass/3DLighting/MaterialProperties.html
//LATON
#define BRASS_KA ((color3){0.329412, 0.223529, 0.027451})
#define BRASS_KD ((color3){0.780392, 0.568627, 0.113725})
#define BRASS_KS ((color3){0.992157, 0.941176, 0.807843})
#define BRASS_NS 27.8974

//BRONCE
#define BRONZE_KA ((color3){0.2125, 0.1275, 0.054})
#define BRONZE_KD ((color3){0.714, 0.4284, 0.18144})
#define BRONZE_KS ((color3){0.393548, 0.271906, 0.166721})
#define BRONZE_NS 25.6

//BRONCE PULIDO
#define POLISHED_BRONZE_KA ((color3){0.25, 0.148, 0.06475})
#define POLISHED_BRONZE_KD ((color3){0.4, 0.2368, 0.1036})
#define POLISHED_BRONZE_KS ((color3){0.774597, 0.458561, 0.200621})
#define POLISHED_BRONZE_NS 76.8

//CROMO
#define CHROME_KA ((color3){0.25, 0.25, 0.25})
#define CHROME_KD ((color3){0.4, 0.4, 0.4})
#define CHROME_KS ((color3){0.774597, 0.774597, 0.774597})
#define CHROME_NS 76.8

//COBRE
#define COPPER_KA ((color3){0.19125, 0.0735, 0.0225})
#define COPPER_KD ((color3){0.7038, 0.27048, 0.0828})
#define COPPER_KS ((color3){0.256777, 0.137622, 0.086014})
#define COPPER_NS 12.8

//COBRE PULIDO
#define POLISHED_COPPER_KA ((color3){0.2295, 0.08825, 0.0275})
#define POLISHED_COPPER_KD ((color3){0.5508, 0.2118, 0.066})
#define POLISHED_COPPER_KS ((color3){0.580594, 0.223257, 0.0695701})
#define POLISHED_COPPER_NS 51.2

// GOLD
#define GOLD_KA ((color3){0.24725, 0.1995, 0.0745})
#define GOLD_KD ((color3){0.75164, 0.60648, 0.22648})
#define GOLD_KS ((color3){0.628281, 0.555802, 0.366065})
#define GOLD_NS 51.2

// POLISHED GOLD
#define POLISHED_GOLD_KA ((color3){0.24725, 0.2245, 0.0645})
#define POLISHED_GOLD_KD ((color3){0.34615, 0.3143, 0.0903})
#define POLISHED_GOLD_KS ((color3){0.797357, 0.723991, 0.208006})
#define POLISHED_GOLD_NS 83.2

// ESTAÑO
#define PEWTER_KA ((color3){0.105882, 0.058824, 0.113725})
#define PEWTER_KD ((color3){0.427451, 0.470588, 0.541176})
#define PEWTER_KS ((color3){0.333333, 0.333333, 0.521569})
#define PEWTER_NS 9.84615

// SILVER
#define SILVER_KA ((color3){0.19225, 0.19225, 0.19225})
#define SILVER_KD ((color3){0.50754, 0.50754, 0.50754})
#define SILVER_KS ((color3){0.508273, 0.508273, 0.508273})
#define SILVER_NS 51.2

// POLISHED SILVER
#define POLISHED_SILVER_KA ((color3){0.23125, 0.23125, 0.23125})
#define POLISHED_SILVER_KD ((color3){0.2775, 0.2775, 0.2775})
#define POLISHED_SILVER_KS ((color3){0.773911, 0.773911, 0.773911})
#define POLISHED_SILVER_NS 89.6

// EMERALD
#define EMERALD_KA ((color3){0.0215, 0.1745, 0.0215})
#define EMERALD_KD ((color3){0.07568, 0.61424, 0.07568})
#define EMERALD_KS ((color3){0.633, 0.727811, 0.633})
#define EMERALD_NS 76.8

// JADE
#define JADE_KA ((color3){0.135, 0.2225, 0.1575})
#define JADE_KD ((color3){0.54, 0.89, 0.63})
#define JADE_KS ((color3){0.316228, 0.316228, 0.316228})
#define JADE_NS 12.8

// OBSIDIAN
#define OBSIDIAN_KA ((color3){0.05375, 0.05, 0.06625})
#define OBSIDIAN_KD ((color3){0.18275, 0.17, 0.22525})
#define OBSIDIAN_KS ((color3){0.332741, 0.328634, 0.346435})
#define OBSIDIAN_NS 38.4

// PEARL
#define PEARL_KA ((color3){0.25, 0.20725, 0.20725})
#define PEARL_KD ((color3){1.0, 0.829, 0.829})
#define PEARL_KS ((color3){0.296648, 0.296648, 0.296648})
#define PEARL_NS 11.264

// RUBY
#define RUBY_KA ((color3){0.1745, 0.01175, 0.01175})
#define RUBY_KD ((color3){0.61424, 0.04136, 0.04136})
#define RUBY_KS ((color3){0.727811, 0.626959, 0.626959})
#define RUBY_NS 76.8

// TURQUOISE
#define TURQUOISE_KA ((color3){0.1, 0.18725, 0.1745})
#define TURQUOISE_KD ((color3){0.396, 0.74151, 0.69102})
#define TURQUOISE_KS ((color3){0.297254, 0.30829, 0.306678})
#define TURQUOISE_NS 12.8

// BLACK PLASTIC
#define BLACK_PLASTIC_KA ((color3){0.0, 0.0, 0.0})
#define BLACK_PLASTIC_KD ((color3){0.01, 0.01, 0.01})
#define BLACK_PLASTIC_KS ((color3){0.50, 0.50, 0.50})
#define BLACK_PLASTIC_NS 32

// BLACK RUBBER
#define BLACK_RUBBER_KA ((color3){0.02, 0.02, 0.02})
#define BLACK_RUBBER_KD ((color3){0.01, 0.01, 0.01})
#define BLACK_RUBBER_KS ((color3){0.4, 0.4, 0.4})
#define BLACK_RUBBER_NS 10
//////////////////////////////////////////////////////////////////////

//// DEFINICION DE LOS MATERIALES PARA LOS OBJETOS ////
#define CONCAT2(x, y) x ## y
#define CONCAT(x, y) CONCAT2(x, y)

#define MATERIAL_R_FALKE RUBY
#define KA_R_FALKE CONCAT(MATERIAL_R_FALKE, _KA)
#define KD_R_FALKE CONCAT(MATERIAL_R_FALKE, _KD)
#define KS_R_FALKE CONCAT(MATERIAL_R_FALKE, _KS)
#define NS_R_FALKE CONCAT(MATERIAL_R_FALKE, _NS)

#define MATERIAL_X_WING EMERALD
#define KA_X_WING CONCAT(MATERIAL_X_WING, _KA)
#define KD_X_WING CONCAT(MATERIAL_X_WING, _KD)
#define KS_X_WING CONCAT(MATERIAL_X_WING, _KS)
#define NS_X_WING CONCAT(MATERIAL_X_WING, _NS)

#define MATERIAL_CAM OBSIDIAN
#define KA_CAM CONCAT(MATERIAL_CAM, _KA)
#define KD_CAM CONCAT(MATERIAL_CAM, _KD)
#define KS_CAM CONCAT(MATERIAL_CAM, _KS)
#define NS_CAM CONCAT(MATERIAL_CAM, _NS)
////////////////////////////////////////////////////////

#define NUM_LIGHTS  5
enum { AMBIENTAL, SUN, BULB, SEL_OBJ, SEL_CAM };
extern light lights[NUM_LIGHTS];

enum { FACES, VERTICES };
extern bool take_normals_of;

//el tercero en discordia: CAMARA, OBJETO y LUZ (para transformation_to, 'c')
#define LUZ 2

//cuanto incrementa/decrementa la apertura de una luz de tipo foco
extern real_t delta_aperture_sin; //5º
extern real_t delta_aperture_cos; //5º 
extern real_t max_aperture_cos;   //90º
extern real_t min_aperture_cos;   //0º

void init_lights(void);
color3 calculate_intensity(const punto3 p, const object *const material_info_ptr, const vector N);

#endif // LIGHTS_H