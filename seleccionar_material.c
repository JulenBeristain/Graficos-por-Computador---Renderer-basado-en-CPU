#include <stdio.h>
#include "seleccionar_material.h"
#include "lights.h"

#define LINE_SIZE 100

void seleccionar_material(color3 *ka, color3 *kd, color3 *ks, real_t *ns){
    char line[LINE_SIZE];
    int sel;
    bool is_selected = 0;
    
    printf("\n\tLista de materiales predefinidos:\n\n"
            " 1.-  Laton\n"
            " 2.-  Bronce\n"
            " 3.-  Bronce Pulido\n"
            " 4.-  Cromo\n"
            " 5.-  Cobre\n"
            " 6.-  Cobre Pulido\n"
            " 7.-  Oro\n"
            " 8.-  Oro Pulido\n"
            " 9.-  Estaino\n"
            " 10.- Plata\n"
            " 11.- Plata Pulida\n"
            " 12.- Esmeralda\n"
            " 13.- Jade\n"
            " 14.- Obsidiana\n"
            " 15.- Perla\n"
            " 16.- Rubi\n"
            " 17.- Turquesa\n"
            " 18.- Plastico Negro\n"
            " 19.- Goma Negra\n\n");

    //Mientras no nos introduzca una opción válida seguimos preguntando
    for(;;){
        printf("\tElige un material introduciendo su numero correspondiente:\n");
        if(fgets(line, sizeof(line), stdin) != NULL){
            if(sscanf(line, "%d", &sel) == 1){
                switch(sel){
                    case 1:
                        *ka = BRASS_KA; *kd = BRASS_KD; *ks = BRASS_KS; *ns = BRASS_NS;
                        is_selected = 1;
                        break;
                    case 2:
                        *ka = BRONZE_KA; *kd = BRONZE_KD; *ks = BRONZE_KS; *ns = BRONZE_NS;
                        is_selected = 1;
                        break;
                    case 3:
                        *ka = POLISHED_BRONZE_KA; *kd = POLISHED_BRONZE_KD; *ks = POLISHED_BRONZE_KS; *ns = POLISHED_BRONZE_NS;
                        is_selected = 1;
                        break;
                    case 4:
                        *ka = CHROME_KA; *kd = CHROME_KD; *ks = CHROME_KS; *ns = CHROME_NS;
                        is_selected = 1;
                        break;
                    case 5:
                        *ka = COPPER_KA; *kd = COPPER_KD; *ks = COPPER_KS; *ns = COPPER_NS;
                        is_selected = 1;
                        break;
                    case 6:
                        *ka = POLISHED_COPPER_KA; *kd = POLISHED_COPPER_KD; *ks = POLISHED_COPPER_KS; *ns = POLISHED_COPPER_NS;
                        is_selected = 1;
                        break;
                    case 7:
                        *ka = GOLD_KA; *kd = GOLD_KD; *ks = GOLD_KS; *ns = GOLD_NS;
                        is_selected = 1;
                        break;
                    case 8:
                        *ka = POLISHED_GOLD_KA; *kd = POLISHED_GOLD_KD; *ks = POLISHED_GOLD_KS; *ns = POLISHED_GOLD_NS;
                        is_selected = 1;
                        break;
                    case 9:
                        *ka = PEWTER_KA; *kd = PEWTER_KD; *ks = PEWTER_KS; *ns = PEWTER_NS;
                        is_selected = 1;
                        break;
                    case 10:
                        *ka = SILVER_KA; *kd = SILVER_KD; *ks = SILVER_KS; *ns = SILVER_NS;
                        is_selected = 1;
                        break;
                    case 11:
                        *ka = POLISHED_SILVER_KA; *kd = POLISHED_SILVER_KD; *ks = POLISHED_SILVER_KS; *ns = POLISHED_SILVER_NS;
                        is_selected = 1;
                        break;
                    case 12:
                        *ka = EMERALD_KA; *kd = EMERALD_KD; *ks = EMERALD_KS; *ns = EMERALD_NS;
                        is_selected = 1;
                        break;
                    case 13:
                        *ka = JADE_KA; *kd = JADE_KD; *ks = JADE_KS; *ns = JADE_NS;
                        is_selected = 1;
                        break;
                    case 14:
                        *ka = OBSIDIAN_KA; *kd = OBSIDIAN_KD; *ks = OBSIDIAN_KS; *ns = OBSIDIAN_NS;
                        is_selected = 1;
                        break;
                    case 15:
                        *ka = PEARL_KA; *kd = PEARL_KD; *ks = PEARL_KS; *ns = PEARL_NS;
                        is_selected = 1;
                        break;
                    case 16:
                        *ka = RUBY_KA; *kd = RUBY_KD; *ks = RUBY_KS; *ns = RUBY_NS;
                        is_selected = 1;
                        break;
                    case 17:
                        *ka = TURQUOISE_KA; *kd = TURQUOISE_KD; *ks = TURQUOISE_KS; *ns = TURQUOISE_NS;
                        is_selected = 1;
                        break;
                    case 18:
                        *ka = BLACK_PLASTIC_KA; *kd = BLACK_PLASTIC_KD; *ks = BLACK_PLASTIC_KS; *ns = BLACK_PLASTIC_NS;
                        is_selected = 1;
                        break;
                    case 19:
                        *ka = BLACK_RUBBER_KA; *kd = BLACK_RUBBER_KD; *ks = BLACK_RUBBER_KS; *ns = BLACK_RUBBER_NS;
                        is_selected = 1;
                        break;
                    default:
                        break;
                }
            }
        }
        
        if(is_selected) { return; }
    }
}